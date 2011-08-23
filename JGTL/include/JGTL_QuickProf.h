/************************************************************************
* QuickProf                                                             *
* Copyright (C) 2006-2007                                               *
* Tyler Streeter  http://www.tylerstreeter.net  tylerstreeter@gmail.com *
* All rights reserved.                                                  *
* Web: http://quickprof.sourceforge.net                                 *
*                                                                       *
* This library is free software; you can redistribute it and/or         *
* modify it under the terms of EITHER:                                  *
*   (1) The GNU Lesser General Public License as published by the Free  *
*       Software Foundation; either version 2.1 of the License, or (at  *
*       your option) any later version. The text of the GNU Lesser      *
*       General Public License is included with this library in the     *
*       file license-LGPL.txt.                                          *
*   (2) The BSD-style license that is included with this library in     *
*       the file license-BSD.txt.                                       *
*   (3) The zlib/libpng license that is included with this library in   *
*       the file license-zlib-libpng.txt.                               *
*                                                                       *
* This library is distributed in the hope that it will be useful,       *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
* license-LGPL.txt, license-BSD.txt, and license-zlib-libpng.txt for    *
* more details.                                                         *
************************************************************************/

// Please visit the project website (http://quickprof.sourceforge.net)
// for usage instructions.

// Edits made by Jason Gauci

#ifndef QUICK_PROF_H
#define QUICK_PROF_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <limits.h>

#ifndef _MSC_VER
using namespace std; //Hack to get around the fact that win32 does not use std:: for min and max
#endif

#include "JGTL_LocatedException.h"
#include "JGTL_DynamicPoolMap.h"

#if defined(WIN32) || defined(_WIN32)
#define USE_WINDOWS_TIMERS
#include <windows.h>
#include <time.h>
#else
#include <sys/time.h>
#endif

/// Use this macro to access the profiler singleton.  For example:
/// PROFILER.init();
/// ...
/// PROFILER.beginBlock("foo");
/// foo();
/// PROFILER.endBlock("foo");
#define PROFILER JGTL::Profiler::getInstance()

/// The main namespace that contains everything.
namespace JGTL
{
	/// A simple data structure representing a single timed block
	/// of code.
	struct ProfileBlock
	{
		ProfileBlock()
		{
			currentBlockStartMicroseconds = 0;
			currentCycleTotalMicroseconds = 0;
			avgCycleTotalMicroseconds = 0;
			smallestCycleMicroseconds = UINT_MAX;
			largestCycleMicroseconds = 0;
			totalMicroseconds = 0;
			smallestCyclePercent=10000.0;
			largestCyclePercent=0.0;
		}

		/// The starting time (in us) of the current block update.
		unsigned long int currentBlockStartMicroseconds;

		/// The accumulated time (in us) spent in this block during the
		/// current profiling cycle.
		unsigned long int currentCycleTotalMicroseconds;

		/// The accumulated time (in us) spent in this block during the
		/// past profiling cycle.
		double avgCycleTotalMicroseconds;

		/// The total accumulated time (in us) spent in this block.
		unsigned long int totalMicroseconds;

		/// The best time for this block
		unsigned long int smallestCycleMicroseconds;
		double smallestCyclePercent;

		/// The worst time for this block
		unsigned long int largestCycleMicroseconds;
		double largestCyclePercent;
	};

	/// A cross-platform clock class inspired by the Timer classes in
	/// Ogre (http://www.ogre3d.org).
	class Clock
	{
	public:
		Clock()
		{
#ifdef USE_WINDOWS_TIMERS
			QueryPerformanceFrequency(&mClockFrequency);
#endif
			reset();
		}

		~Clock()
		{
		}

		/**
		Resets the initial reference time.
		*/
		void reset()
		{
#ifdef USE_WINDOWS_TIMERS
			QueryPerformanceCounter(&mStartTime);
			mStartTick = GetTickCount();
			mPrevElapsedTime = 0;
#else
			gettimeofday(&mStartTime, NULL);
#endif
		}

		/**
		Returns the time in us since the last call to reset or since
		the Clock was created.

		@return The requested time in milliseconds.
		*/
		unsigned long int getTimeMilliseconds()
		{
#ifdef USE_WINDOWS_TIMERS
			LARGE_INTEGER currentTime;
			QueryPerformanceCounter(&currentTime);
			LONGLONG elapsedTime = currentTime.QuadPart -
				mStartTime.QuadPart;

			// Compute the number of millisecond ticks elapsed.
			unsigned long msecTicks = (unsigned long)(1000 * elapsedTime /
				mClockFrequency.QuadPart);

			// Check for unexpected leaps in the Win32 performance counter.
			// (This is caused by unexpected data across the PCI to ISA
			// bridge, aka south bridge.  See Microsoft KB274323.)
			unsigned long elapsedTicks = GetTickCount() - mStartTick;
			signed long msecOff = (signed long)(msecTicks - elapsedTicks);
			if (msecOff < -100 || msecOff > 100)
			{
				// Adjust the starting time forwards.
				LONGLONG msecAdjustment = (std::min)(msecOff *
					mClockFrequency.QuadPart / 1000, elapsedTime -
					mPrevElapsedTime);
				mStartTime.QuadPart += msecAdjustment;
				elapsedTime -= msecAdjustment;

				// Recompute the number of millisecond ticks elapsed.
				msecTicks = (unsigned long)(1000 * elapsedTime /
					mClockFrequency.QuadPart);
			}

			// Store the current elapsed time for adjustments next time.
			mPrevElapsedTime = elapsedTime;

			return msecTicks;
#else
			struct timeval currentTime;
			gettimeofday(&currentTime, NULL);
			return (currentTime.tv_sec - mStartTime.tv_sec) * 1000 +
				(currentTime.tv_usec - mStartTime.tv_usec) / 1000;
#endif
		}

		/**
		Returns the time in us since the last call to reset or since
		the Clock was created.

		@return The requested time in microseconds.
		*/
		unsigned long int getTimeMicroseconds()
		{
#ifdef USE_WINDOWS_TIMERS
			LARGE_INTEGER currentTime;
			QueryPerformanceCounter(&currentTime);
			LONGLONG elapsedTime = currentTime.QuadPart -
				mStartTime.QuadPart;

			// Compute the number of millisecond ticks elapsed.
			unsigned long msecTicks = (unsigned long)(1000 * elapsedTime /
				mClockFrequency.QuadPart);

			// Check for unexpected leaps in the Win32 performance counter.
			// (This is caused by unexpected data across the PCI to ISA
			// bridge, aka south bridge.  See Microsoft KB274323.)
			unsigned long elapsedTicks = GetTickCount() - mStartTick;
			signed long msecOff = (signed long)(msecTicks - elapsedTicks);
			if (msecOff < -100 || msecOff > 100)
			{
				// Adjust the starting time forwards.
				LONGLONG msecAdjustment = (std::min)(msecOff *
					mClockFrequency.QuadPart / 1000, elapsedTime -
					mPrevElapsedTime);
				mStartTime.QuadPart += msecAdjustment;
				elapsedTime -= msecAdjustment;
			}

			// Store the current elapsed time for adjustments next time.
			mPrevElapsedTime = elapsedTime;

			// Convert to microseconds.
			unsigned long usecTicks = (unsigned long)(1000000 * elapsedTime /
				mClockFrequency.QuadPart);

			return usecTicks;
#else
			struct timeval currentTime;
			gettimeofday(&currentTime, NULL);
			return (currentTime.tv_sec - mStartTime.tv_sec) * 1000000 +
				(currentTime.tv_usec - mStartTime.tv_usec);
#endif
		}

	private:
#ifdef USE_WINDOWS_TIMERS
		LARGE_INTEGER mClockFrequency;
		DWORD mStartTick;
		LONGLONG mPrevElapsedTime;
		LARGE_INTEGER mStartTime;
#else
		struct timeval mStartTime;
#endif
	};

	/// A set of ways to represent timing results.
	enum TimeFormat
	{
		SECONDS,
		MILLISECONDS,
		MICROSECONDS,
		PERCENT
	};

	/// A singleton class that manages timing for a set of profiling blocks.
	class Profiler
	{
	public:
		inline Profiler();

		virtual inline ~Profiler();

		/**
		Initializes the profiler.

		This must be called first.  If this is never called, the profiler
		is effectively disabled, and all other functions will return
		immediately.

		@param smoothing      The measured duration for each profile
		block can be averaged across multiple
		cycles, and this parameter defines the
		smoothness of this averaging process.
		The higher the value, the smoother the
		resulting average durations will appear.
		Leaving it at zero will essentially
		disable the smoothing effect.  More
		specifically, this parameter is a time
		constant (defined in terms of cycles) that
		defines an exponentially-weighted moving
		average.  For example, a value of 4.0
		means the past four cycles will contribute
		63% of the current weighted average.  This
		value must be >= 0.
		@param outputFilename If defined, enables timing data to be
		printed to a data file for later analysis.
		@param printPeriod    Defines how often data is printed to the
		file, in number of profiling cycles.  For
		example, set this to 1 if you want data
		printed after each cycle, or 5 if you want
		it printed every 5 cycles.  It is a good
		idea to increase this if you don't want
		huge data files.  Keep in mind, however,
		that when you increase this, you might
		want to increase the smoothing
		parameter.  (A good heuristic is to set
		the smoothing parameter equal to the
		print period.)  This value must be >= 1.
		@param printFormat    Defines the format used when printing data
		to a file.
		*/
		inline void init(double smoothing=0.0,
			const std::string outputFilename="", size_t printPeriod=1,
			TimeFormat printFormat=MILLISECONDS);

		inline void reset()
		{
			mProfileBlocks.clear();

			mCurrentCycleStartMicroseconds = 0;
			mLastCycleDurationMicroseconds = 0;
			mFirstFileOutput = true;
			mMovingAvgScalar = 0;
			mPrintPeriod = 1;
			mPrintFormat = SECONDS;
			mCycleCounter = 0;
			mFirstCycle = true;
			microsecondsSinceInit = 0;

			mClock.reset();

			// Set the start time for the first cycle.
			mCurrentCycleStartMicroseconds = mClock.getTimeMicroseconds();
		}

		/**
		Begins timing the named block of code.

		@param name The name of the block.
		*/
		inline void beginBlock(const std::string& name);

		/**
		Defines the end of the named timing block.

		@param name The name of the block.
		*/
		inline void endBlock(const std::string& name);

		inline void beginCycle();

		/**
		Defines the end of a profiling cycle.

		Use this regularly by calling it at the end of all timing blocks.
		This is necessary for smoothing and for file output, but not if
		you just want a total summary at the end of execution (i.e. from
		getSummary).  This must not be called within a timing block.
		*/
		inline void endCycle();

		/**
		Returns the average time used in the named block per profiling cycle.

		If smoothing is disabled (see init), this returns the most recent
		duration measurement.

		@param name   The name of the block.
		@param format The desired time format to use for the result.
		@return       The block's average duration per cycle.
		*/
		inline double getAvgDuration(const std::string& name,
			TimeFormat format);

		/**
		Returns a summary of total times in each block.

		@param format The desired time format to use for the results.
		@return       The timing summary as a string.
		*/
		inline std::string getSummary(TimeFormat format=PERCENT);

        inline bool isInCycle() const
        {
            return inCycle;
        }

	protected:
		/**
		Prints an error message to standard output.

		@param msg The string to print.
		*/
		inline void printError(const std::string& msg);

		/**
		Returns a named profile block.

		@param name The name of the block to return.
		@return     The named ProfileBlock, or NULL if it can't be found.
		*/
		inline ProfileBlock* getProfileBlock(const std::string& name);

		/**
		Returns the time spent in the named block since the profiler was
		initialized.

		@param name   The name of the block.
		@param format The desired time format to use for the result.
		@return       The block total time.
		*/
		inline double getBlockMinTime(const std::string &name,
			TimeFormat format);

		/**
		Returns the time spent in the named block since the profiler was
		initialized.

		@param name   The name of the block.
		@param format The desired time format to use for the result.
		@return       The block total time.
		*/
		inline double getBlockTotalTime(const std::string &name,
			TimeFormat format);

		/**
		Returns the time spent in the named block since the profiler was
		initialized.

		@param name   The name of the block.
		@param format The desired time format to use for the result.
		@return       The block total time.
		*/
		inline double getBlockMaxTime(const std::string &name,
			TimeFormat format);

		/**
		Computes the elapsed time since the profiler was initialized.

		@return The elapsed time in microseconds.
		*/
		inline double getMicrosecondsSinceInit();

		/**
		Returns the appropriate suffix string for the given time format.

		@return The suffix string.
		*/
		inline std::string getSuffixString(TimeFormat format);

		/// Determines whether the profiler is enabled.
		bool mEnabled;

		/// The clock used to time profile blocks.
		Clock mClock;

		/// The starting time (in us) of the current profiling cycle.
		unsigned long int mCurrentCycleStartMicroseconds;

		/// The duration (in us) of the most recent profiling cycle.
		unsigned long int mLastCycleDurationMicroseconds;

		/// Internal map of named profile blocks.
		DynamicPoolMap<std::string, ProfileBlock*> mProfileBlocks;

		/// The data output file used if this feature is enabled in init.
		std::ofstream mOutputFile;

		/// Tracks whether we have begun printing data to the output file.
		bool mFirstFileOutput;

		/// A pre-computed scalar used to update exponentially-weighted moving
		/// averages.
		double mMovingAvgScalar;

		/// Determines how often (in number of profiling cycles) timing data
		/// is printed to the output file.
		size_t mPrintPeriod;

		/// The time format used when printing timing data to the output file.
		TimeFormat mPrintFormat;

		/// Keeps track of how many cycles have elapsed (for printing).
		size_t mCycleCounter;

		/// Used to update the initial average cycle times.
		bool mFirstCycle;

		unsigned long microsecondsSinceInit;

        bool inCycle;
	};

	Profiler::Profiler()
		:
		mProfileBlocks(65536),
            inCycle(false)
	{
		mEnabled = false;
		mCurrentCycleStartMicroseconds = 0;
		mLastCycleDurationMicroseconds = 0;
		mFirstFileOutput = true;
		mMovingAvgScalar = 0;
		mPrintPeriod = 1;
		mPrintFormat = SECONDS;
		mCycleCounter = 0;
		mFirstCycle = true;
		microsecondsSinceInit = 0;
	}

	Profiler::~Profiler()
	{
		if (mOutputFile.is_open())
		{
			mOutputFile.close();
		}

		// Destroy all ProfileBlocks.
		while (!mProfileBlocks.empty())
		{
			delete (*mProfileBlocks.begin()).second;
			mProfileBlocks.erase(mProfileBlocks.begin());
		}
	}

	void Profiler::init(double smoothing, const std::string outputFilename,
		size_t printPeriod, TimeFormat printFormat)
	{
		if (mEnabled)
		{
			printError("Cannot init the profiler multiple times.");
			return;
		}

		mEnabled = true;

		if (smoothing <= 0)
		{
			if (smoothing < 0)
			{
				printError("Smoothing parameter must be >= 0. Using 0.");
			}

			mMovingAvgScalar = 0;
		}
		else
		{
			// Treat smoothing as a time constant.
			mMovingAvgScalar = ::exp(-1 / smoothing);
		}

		if (!outputFilename.empty())
		{
			mOutputFile.open(outputFilename.c_str());
		}

		if (printPeriod < 1)
		{
			printError("Print period must be >= 1. Using 1.");
			mPrintPeriod = 1;
		}
		else
		{
			mPrintPeriod = printPeriod;
		}
		mPrintFormat = printFormat;

		mClock.reset();

		// Set the start time for the first cycle.
		mCurrentCycleStartMicroseconds = mClock.getTimeMicroseconds();
	}

	void Profiler::beginBlock(const std::string& name)
	{
		if (!mEnabled || !inCycle)
		{
			return;
		}

		if (name.empty())
		{
			printError("Cannot allow unnamed profile blocks.");
			return;
		}

		ProfileBlock** blockPtr = mProfileBlocks.getData(name);
		ProfileBlock* block;

		if (!blockPtr)
		{
			// Create a new ProfileBlock.
			mProfileBlocks.insert(name,new ProfileBlock());
			block = mProfileBlocks.getDataRef(name);
		}
		else
		{
			block = (*blockPtr);
		}

		// We do this at the end to get more accurate results.
		block->currentBlockStartMicroseconds = mClock.getTimeMicroseconds();
	}

	void Profiler::endBlock(const std::string& name)
	{
		if (!mEnabled || !inCycle)
		{
			return;
		}

		// We do this at the beginning to get more accurate results.
		unsigned long int endTick = mClock.getTimeMicroseconds();

		ProfileBlock* block = getProfileBlock(name);
		if (!block)
		{
			return;
		}

		unsigned long int blockDuration =
			endTick -
			block->currentBlockStartMicroseconds;

		block->currentCycleTotalMicroseconds += blockDuration;

		block->totalMicroseconds += blockDuration;
	}

	void Profiler::beginCycle()
	{
		if (!mEnabled)
		{
			return;
		}

		mCurrentCycleStartMicroseconds = mClock.getTimeMicroseconds();

		//Clear out any accumulated time in all blocks
		//Remove total time for any blocks have ended before the cycle began.
		JGTL::DynamicPoolMap<std::string, ProfileBlock*>::iterator iter;
		for (iter = mProfileBlocks.begin(); iter != mProfileBlocks.end(); ++iter)
		{
			ProfileBlock* block = (*iter).second;

			block->totalMicroseconds -= block->currentCycleTotalMicroseconds;
			block->currentCycleTotalMicroseconds = 0;
		}

        inCycle=true;
    }

	void Profiler::endCycle()
	{
		if (!mEnabled || !inCycle)
		{
			return;
		}

		// Store the duration of the cycle that just finished.
		mLastCycleDurationMicroseconds = mClock.getTimeMicroseconds() -
			mCurrentCycleStartMicroseconds;

		microsecondsSinceInit += mLastCycleDurationMicroseconds;

		// Update the average cycle time for each block.
		JGTL::DynamicPoolMap<std::string, ProfileBlock*>::iterator iter;
		for (iter = mProfileBlocks.begin(); iter != mProfileBlocks.end(); ++iter)
		{
			ProfileBlock* block = (*iter).second;

			// On the first cycle we set the average cycle time equal to the
			// measured cycle time.  This avoids having to ramp up the average
			// from zero initially.
			if (mFirstCycle)
			{
				block->avgCycleTotalMicroseconds =
					(double)block->currentCycleTotalMicroseconds;
			}
			else
			{
				block->avgCycleTotalMicroseconds = mMovingAvgScalar *
					(double)block->avgCycleTotalMicroseconds + (1 -
					mMovingAvgScalar) *
					(double)block->currentCycleTotalMicroseconds;
			}

			if(block->currentCycleTotalMicroseconds)
			{
				block->smallestCycleMicroseconds =
					min(block->smallestCycleMicroseconds,block->currentCycleTotalMicroseconds);
				block->largestCycleMicroseconds =
					max(block->largestCycleMicroseconds,block->currentCycleTotalMicroseconds);

				double tmpPercent = double(block->currentCycleTotalMicroseconds) /
					mLastCycleDurationMicroseconds;
				block->smallestCyclePercent =
					min(block->smallestCyclePercent,tmpPercent);
				block->largestCyclePercent =
					max(block->largestCyclePercent,tmpPercent);

				block->currentCycleTotalMicroseconds = 0;
			}
		}

		if (mFirstCycle)
		{
			mFirstCycle = false;
		}

		// If enough cycles have passed, print data to the output file.
		if (mOutputFile.is_open() && mCycleCounter % mPrintPeriod == 0)
		{
			mCycleCounter = 0;

			if (mFirstFileOutput)
			{
				// On the first iteration, print a header line that shows the
				// names of each data column (i.e. profiling block names).
				mOutputFile << "# t(s)";

				std::string suffix = getSuffixString(mPrintFormat);
				for (iter = mProfileBlocks.begin(); iter != mProfileBlocks.end();
					++iter)
				{
					mOutputFile  << " " << (*iter).first << "(" << suffix << ")";
				}

				mOutputFile << std::endl;
				mFirstFileOutput = false;
			}

			// Print the total elapsed time in seconds.
			mOutputFile << getMicrosecondsSinceInit() * (double)0.000001;

			// Print the cycle time for each block.
			for (iter = mProfileBlocks.begin(); iter != mProfileBlocks.end();
				++iter)
			{
				mOutputFile << " " << getAvgDuration((*iter).first,
					mPrintFormat);
			}

			mOutputFile << std::endl;
		}

		++mCycleCounter;
		mCurrentCycleStartMicroseconds = mClock.getTimeMicroseconds();

        inCycle=false;
	}

	double Profiler::getBlockMinTime(const std::string &name,
		TimeFormat format)
	{
		if (!mEnabled)
		{
			return 0;
		}

		ProfileBlock* block = getProfileBlock(name);
		if (!block)
		{
			return 0;
		}

		double blockTotalMicroseconds = (double)block->smallestCycleMicroseconds;
		double result = 0;

		switch(format)
		{
		case SECONDS:
			result = blockTotalMicroseconds * (double)0.000001;
			break;
		case MILLISECONDS:
			result = blockTotalMicroseconds * (double)0.001;
			break;
		case MICROSECONDS:
			result = blockTotalMicroseconds;
			break;
		case PERCENT:
			{
				double microsecondsSinceInit = getMicrosecondsSinceInit();
				if (0 == microsecondsSinceInit)
				{
					result = 0;
				}
				else
				{
					result = 100 * block->smallestCyclePercent;
				}
			}
			break;
		default:
			break;
		}

		return result;
	}

	double Profiler::getBlockTotalTime(const std::string& name,
		TimeFormat format)
	{
		if (!mEnabled)
		{
			return 0;
		}

		ProfileBlock* block = getProfileBlock(name);
		if (!block)
		{
			return 0;
		}

		double blockTotalMicroseconds = (double)block->totalMicroseconds;
		double result = 0;

		switch(format)
		{
		case SECONDS:
			result = blockTotalMicroseconds * (double)0.000001;
			break;
		case MILLISECONDS:
			result = blockTotalMicroseconds * (double)0.001;
			break;
		case MICROSECONDS:
			result = blockTotalMicroseconds;
			break;
		case PERCENT:
			{
				double microsecondsSinceInit = getMicrosecondsSinceInit();
				if (0 == microsecondsSinceInit)
				{
					result = 0;
				}
				else
				{
					result = 100.0 * blockTotalMicroseconds /
						microsecondsSinceInit;
				}
			}
			break;
		default:
			break;
		}

		return result;
	}

	double Profiler::getBlockMaxTime(const std::string &name,
		TimeFormat format)
	{
		if (!mEnabled)
		{
			return 0;
		}

		ProfileBlock* block = getProfileBlock(name);
		if (!block)
		{
			return 0;
		}

		double blockMaxMicroseconds = (double)block->largestCycleMicroseconds;
		double result = 0;

		switch(format)
		{
		case SECONDS:
			result = blockMaxMicroseconds * (double)0.000001;
			break;
		case MILLISECONDS:
			result = blockMaxMicroseconds * (double)0.001;
			break;
		case MICROSECONDS:
			result = blockMaxMicroseconds;
			break;
		case PERCENT:
			{
				double microsecondsSinceInit = getMicrosecondsSinceInit();
				if (0 == microsecondsSinceInit)
				{
					result = 0;
				}
				else
				{
					result = 100 * block->largestCyclePercent;
				}
			}
			break;
		default:
			break;
		}

		return result;
	}

	double Profiler::getAvgDuration(const std::string& name,
		TimeFormat format)
	{
		if (!mEnabled)
		{
			return 0;
		}

		ProfileBlock* block = getProfileBlock(name);
		if (!block)
		{
			return 0;
		}

		double result = 0;

		switch(format)
		{
		case SECONDS:
			result = block->avgCycleTotalMicroseconds * (double)0.000001;
			break;
		case MILLISECONDS:
			result = block->avgCycleTotalMicroseconds * (double)0.001;
			break;
		case MICROSECONDS:
			result = block->avgCycleTotalMicroseconds;
			break;
		case PERCENT:
			{
				if (0 == mLastCycleDurationMicroseconds)
				{
					result = 0;
				}
				else
				{
					result = 100.0 * block->avgCycleTotalMicroseconds /
						mLastCycleDurationMicroseconds;
				}
				break;
			}
		default:
			break;
		}

		return result;
	}

	std::string Profiler::getSummary(TimeFormat format)
	{
		if (!mEnabled)
		{
			return "";
		}

		std::ostringstream oss;
		std::string suffix = getSuffixString(format);

		JGTL::DynamicPoolMap<std::string, ProfileBlock*>::iterator iter;
		for (iter = mProfileBlocks.begin(); iter != mProfileBlocks.end(); ++iter)
		{
			if (iter != mProfileBlocks.begin())
			{
				oss << "\n";
			}

			oss << (*iter).first;
			oss << ": ";
			oss << getBlockMinTime((*iter).first, format);
			oss << " ";
			oss << suffix;
			oss << " min, ";
			oss << getAvgDuration((*iter).first, format);
			oss << " ";
			oss << suffix;
			oss << " avg, ";
			oss << getBlockMaxTime((*iter).first, format);
			oss << " ";
			oss << suffix;
			oss << " max.";

		}

		return oss.str();
	}

	void Profiler::printError(const std::string& msg)
	{
		std::cout << "[QuickProf error] " << msg << std::endl;
	}

	ProfileBlock* Profiler::getProfileBlock(const std::string& name)
	{
		ProfileBlock* block = mProfileBlocks.getDataRef(name);

		if (!block)
		{
			// The named block does not exist.  Print an error.
			printError("The profile block named '" + name +
				"' does not exist.");
		}

		return block;
	}

	double Profiler::getMicrosecondsSinceInit()
	{
		double timeSinceInit = (double)microsecondsSinceInit;

		if (timeSinceInit < 0)
		{
			timeSinceInit = 0;
		}

		return timeSinceInit;
	}

	std::string Profiler::getSuffixString(TimeFormat format)
	{
		std::string suffix;
		switch(format)
		{
		case SECONDS:
			suffix = "s";
			break;
		case MILLISECONDS:
			suffix = "ms";
			break;
		case MICROSECONDS:
			suffix = "us";
			break;
		case PERCENT:
			{
				suffix = "%";
				break;
			}
		default:
			break;
		}

		return suffix;
	}

	class ProfileBlockHandler
	{
	public:
	protected:
		const char* blockName;
        Profiler* profiler;
	public:
		ProfileBlockHandler(Profiler* _profiler,const char* _blockName)
			:
		blockName(_blockName),
        profiler(_profiler)
		{
			profiler->beginBlock(blockName);
		}

		~ProfileBlockHandler()
		{
			profiler->endBlock(blockName);
		}
	protected:
	};

}

#endif
