#ifndef XORSPACE_H_INCLUDED
#define XORSPACE_H_INCLUDED

#include <algorithm>
#include <iostream>
#include <cmath>
#include <vector>

#include "JGTL_StringConverter.h"
#include "JGTL_LocatedException.h"

#ifdef USE_DILUCULUM_WRAPPER
/** Diluculum header **/
#include <Diluculum/LuaWrappers.hpp>

using namespace Diluculum;
#endif

//Set this define to allow tinyxmldll support
#ifdef USE_TINYXMLDLL

#include "tinyxmldll.h"

#endif

namespace JGTL
{
	/**
	* @class XorSpaceRect
	* @brief This handles a single Xor Rectangle.
	* @author Jason Gauci
	* @Date 2008
	*/

	template<class Rectangle,class Point>
	class XorSpaceRect
	{
	public:
		Rectangle rect;
		bool positive;

		XorSpaceRect()
			:
		positive(true)
		{}

		XorSpaceRect(Rectangle _rect,bool _positive)
			:
		rect(_rect),
			positive(_positive)
		{}
	};

	/**
	* @class XorSpace
	* @brief This class handles Xor spaces.  Think of this as a way to handle things like
	* rectangular doughnuts.  A positive space followed by a smaller concentric negative space
	* would represent a doughnut.
	* @author Jason Gauci
	* @Date 2008
	*/

	template<class Rectangle,class Point>
	class XorSpace
	{
	public:
		typedef typename std::vector< XorSpaceRect<Rectangle,Point> >::iterator SpaceIterator;

		typedef typename std::vector< XorSpaceRect<Rectangle,Point> >::const_iterator ConstSpaceIterator;

	protected:
		std::vector<XorSpaceRect<Rectangle,Point> > spaces;
		Point topLeft,bottomRight;

		//typedef std::vector<XorSpaceRect<Rectangle,Point> >::iterator SpaceIterator;

	public:
		XorSpace()
		{}

		XorSpace(const Rectangle &start)
		{
			addSpace(start,true);
		}

#ifdef USE_TINYXMLDLL
		XorSpace(TiXmlElement *root)
		{
			TiXmlElement *child = root->FirstChildElement("Space");

			Rectangle rect;
			bool positive;

			while (child)
			{
				rect.topLeft = stringTo<Point>(child->FirstChildElement("TopLeft")->GetText());
				rect.size = stringTo<Point>(child->FirstChildElement("Size")->GetText());
				positive = stringTo<bool>(child->FirstChildElement("Positive")->GetText());

				addSpace(rect,positive);

				child = child->NextSiblingElement("Space");
			}
		}

		void save(TiXmlElement *root)
		{
			for (int a=0;a<(int)spaces.size();a++)
			{
				TiXmlElement *child = new TiXmlElement("Space");

				TiXmlElement *child2;

				child2 = new TiXmlElement("TopLeft");
				child2->LinkEndChild(new TiXmlText(toString(spaces[a].rect.topLeft)));
				child->LinkEndChild(child2);


				child2 = new TiXmlElement("Size");
				child2->LinkEndChild(new TiXmlText(toString(spaces[a].rect.size)));
				child->LinkEndChild(child2);


				child2 = new TiXmlElement("Positive");
				child2->LinkEndChild(new TiXmlText(toString(spaces[a].positive)));
				child->LinkEndChild(child2);

				root->LinkEndChild(child);
			}
		}
#endif

		/**
		* Adds a space with the highest priority to the list of spaces.
		* @param space The rectangle to add
		* @param positive Whether the rectangle is positive or negative (a negative cancels all positives before it)
		*/
		void addSpace(const Rectangle &space,bool positive)
		{
			spaces.push_back(XorSpaceRect<Rectangle,Point>(space,positive));

			if (spaces.size()==1)
			{
				if (!positive)
				{
					throw CREATE_LOCATEDEXCEPTION_INFO("You should always start an XorSpace with a positive rectangle!");
				}

				topLeft = space.topLeft;
				bottomRight = space.topLeft + space.size;
			}
			else
			{
				if (positive)
				{
					topLeft.x = min(topLeft.x,space.topLeft.x);
					topLeft.y = min(topLeft.y,space.topLeft.y);
					topLeft.z = min(topLeft.z,space.topLeft.z);

					bottomRight.x = max(bottomRight.x,space.topLeft.x+space.size.x);
					bottomRight.y = max(bottomRight.y,space.topLeft.y+space.size.y);
					bottomRight.z = max(bottomRight.z,space.topLeft.z+space.size.z);
				}
			}

			pack();
		}

		/**
		* Removes a space with the lowest priority and a certain size and polarity
		* @param space The rectangle to add
		* @param positive Whether the rectangle is positive or negative (a negative cancels all positives before it)
		*/
		void removeSpace(const Rectangle &space,bool positive)
		{
			for (
				SpaceIterator spaceIterator = spaces.begin();
				spaceIterator != spaces.end();
			spaceIterator++
				)
			{
				if (spaceIterator->rect == space && spaceIterator->positive = positive)
				{
					spaces.erase(spaceIterator);

					//now, recompute topLeft and bottomRight
					topLeft = spaces[0].topLeft;
					bottomRight = spaces[0].topLeft = spaces[0].size;

					for (int a=0;a<(int)spaces.size();a++)
					{
						topLeft.x = min(topLeft.x,spaces[a].topLeft.x);
						topLeft.y = min(topLeft.y,spaces[a].topLeft.y);
						topLeft.z = min(topLeft.z,spaces[a].topLeft.z);

						bottomRight.x = max(bottomRight.x,spaces[a].topLeft.x+spaces[a].size.x);
						bottomRight.y = max(bottomRight.y,spaces[a].topLeft.y+spaces[a].size.y);
						bottomRight.z = max(bottomRight.z,spaces[a].topLeft.z+spaces[a].size.z);
					}

					return;
				}
			}

			throw CREATE_LOCATEDEXCEPTION_INFO("Error: Tried to remove a space which does not exist!");
		}

		/**
		* Checks if the XorSpace contains a certain point
		*/
		bool contains(const Point &point) const
		{
			for (
				int index = ((int)spaces.size())-1;
				index>=0;
			index--
				)
			{
				if (spaces[index].rect.contains(point))
				{
					return spaces[index].positive;
				}
			}

			return false;
		}

		const Point &getTopLeft() const
		{
			return topLeft;
		}

		const Point &getBottomRight() const
		{
			return bottomRight;
		}

		Point getSize() const
		{
			return bottomRight - topLeft;
		}

		/**
		* Returns the top-left point of an Xor-Space
		*/
		Point getFirstPoint() const
		{
			Point tmpPoint = topLeft;

			if(!contains(tmpPoint))
			{
				if(!getNextPoint(tmpPoint))
				{
					throw CREATE_LOCATEDEXCEPTION_INFO("Error! No points exist!");
				}
			}

			return tmpPoint;
		}

		/**
		* Gets the next point within an XorSpace.  Works along with getFirstPoint() to iterate
		* over all points.
		*/
		bool getNextPoint(Point &curPoint) const
		{
			while (curPoint < bottomRight)
			{
				//Advance the point
				curPoint.x++;
				if (curPoint.x==bottomRight.x)
				{
					curPoint.x=topLeft.x;
					curPoint.y++;

					if (curPoint.y==bottomRight.y)
					{
						curPoint.y=topLeft.y;
						curPoint.z++;
					}
				}

				if (contains(curPoint))
				{
					//We found a valid point!
					return true;
				}

				//No valid point yet, keep lookin'.
			}

			return false;
		}

		/**
		* Tries to eliminate redundant spaces in an XorSpace
		*/
		void pack()
		{
			for (int a=0;a<(int)spaces.size();a++)
			{
				XorSpaceRect<Rectangle,Point> s = spaces[a];
				spaces.erase(spaces.begin()+a);

				bool putSpaceBack=false;

				for (int z=s.rect.topLeft.z;!putSpaceBack&&z<s.rect.topLeft.z+s.rect.size.z;z++)
				{
					for (int y=s.rect.topLeft.y;!putSpaceBack&&y<s.rect.topLeft.y+s.rect.size.y;y++)
					{
						for (int x=s.rect.topLeft.x;!putSpaceBack&&x<s.rect.topLeft.x+s.rect.size.x;x++)
						{
							Point p(x,y,z);

							if (contains(p) != s.positive)
							{
								//The XorSpace got a different result without the space in there.
								//This means that the space is necessary
								putSpaceBack=true;
							}
						}
					}
				}

				if (putSpaceBack)
				{
					spaces.insert(spaces.begin()+a,s);
				}
				else
				{
					//A space is missing, decrement index to compensate.
					a--;
				}
			}
		}

	protected:

	};

	template<class Rectangle,class Point>
	inline ostream& operator<<(ostream& stream, const XorSpace<Rectangle,Point>& d)
	{
		stream << d.topLeft << ' ' << d.bottomRight << ' '
			<< d.spaces.size() << ' ';
		for(size_t a=0;a<d.spaces.size();a++)
		{
			stream << d.spaces[a].rect << ' ' << d.spaces[a].positive << ' ';
		}

		return stream;
	}

	template<class Rectangle,class Point>
	inline istream& operator>>(istream& stream, XorSpace<Rectangle,Point>& d)
	{
		size_t tmpsize;
		stream >> d.topLeft >> d.bottomRight >> tmpsize;
		for(size_t a=0;a<tmpsize;a++)
		{
			stream >> d.spaces[a].rect >> d.spaces[a].positive;
		}

		return stream;
	}
}

#endif

