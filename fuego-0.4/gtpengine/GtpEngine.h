//----------------------------------------------------------------------------
/** @file GtpEngine.h
    Basic implementation of the Go Text Protocol (GTP).

    Depends only on the standard C++ library for maximum reusability.
    If ponder or interrupt functionality is enabled by setting the macros
    GTPENGINE_PONDER and/or GTPENGINE_INTERRUPT to 1, the Boost.Thread library
    is also needed.

    GtpEngine implements a GTP engine with some basic commands. This class
    is typically used as a base class for other GTP engines.
    GtpEngine::Register allows to register additional commands.
    GtpEngine::MainLoop starts the main command loop.
    Command handlers implement the interface GtpCallbackBase.
    For class member functions, such a callback can be constructed with
    the GtpCallback template class.
    Each callback function is passed a GtpCommand argument, which can be
    queried for arguments and used for writing the response to.
    GTP error responses are created by throwing an instance of GtpFailure.
    All such exceptions are caught in the main loop and converted
    into a response with error status.
*/
//----------------------------------------------------------------------------

#ifndef GTPENGINE_H
#define GTPENGINE_H

#include <cstddef>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "GtpInputStream.h"
#include "GtpOutputStream.h"

#ifndef GTPENGINE_PONDER
/** Macro for enabling pondering.
    If this macro is enabled, GtpEngine has the additional functions
    Ponder(), InitPonder() and StopPonder(), which will be called while
    waiting for the next command. This can be used for thinking during the
    opponent's time.
    Enabling this macro adds a dependency on the Boost.Thread library.
    @see GtpEngine::Ponder()
*/
#define GTPENGINE_PONDER 1
#endif

#ifndef GTPENGINE_INTERRUPT
/** Macro for enabling interrupt ability.
    If this macro is enabled, GtpEngine has the additional function
    Interrupt() to interrupt a running command.
    Enabling this macro adds a dependency on the Boost.Thread library.
    @see GtpEngine::Interrupt()
*/
#define GTPENGINE_INTERRUPT 1
#endif

//----------------------------------------------------------------------------

/** GTP failure.
    Command handlers generate a GTP error response by throwing an instance
    of GtpFailure.
    It contains an internal string stream for building the reponse using
    stream output operators.
    To make formatting of responses with a temporary object more convenient,
    operator<< uses non-standard semantics, such that a new object is
    returned.
    Usage examples:
    @verbatim
    // OK. Construct with string
    throw GtpFailure("message");

    // OK. Use temporary object
    throw GtpFailure() << message << ...;

    // NOT OK. Object is not modified, the return value of << is ignored
    GtpFailure failure;
    failure << message << ...;
    throw failure;

    // OK. Use the internal string stream
    GtpFailure failure;
    failure.ResponseStream() << message << ...;
    throw failure;
    @endverbatim
*/
class GtpFailure
{
public:
    /** Construct with no message. */
    GtpFailure();

    /** Construct with message. */
    GtpFailure(const std::string& response);

    /** Copy constructor.
        Needed for operator<<.
        Preserves the internal string stream format state.
    */
    GtpFailure(const GtpFailure& failure);

    /** Destructor. */
    ~GtpFailure() throw();

    /** Get the response.
        Returns a copy of the text in the internal string stream.
    */
    std::string Response() const;

    /** Get the internal string stream. */
    std::ostream& ResponseStream();

private:
    std::ostringstream m_response;
};

/** @relates GtpFailure
    @note Returns a new object, see @ref GtpFailure
*/
template<typename TYPE>
GtpFailure operator<<(const GtpFailure& failure, const TYPE& type)
{
    GtpFailure result(failure);
    result.ResponseStream() << type;
    return result;
}

/** @relates GtpFailure
    @note Returns a new object, see @ref GtpFailure
*/
template<typename TYPE>
GtpFailure operator<<(const GtpFailure& failure, TYPE& type)
{
    GtpFailure result(failure);
    result.ResponseStream() << type;
    return result;
}

inline std::string GtpFailure::Response() const
{
    return m_response.str();
}

inline std::ostream& GtpFailure::ResponseStream()
{
    return m_response;
}

//----------------------------------------------------------------------------



/** GTP command.
    GtpCommands are passed to command handlers.
    They can be queried for arguments and used for writing the response to.

    Arguments can contain spaces if they are double quoted, for instance:
    @verbatim loadsgf "My File.sgf" @endverbatim
    Double quotes in a quoted argument have to be escaped with '\'.

    The response message format does not have any special requirement,
    it will be sanitized by GtpEngine before writing to form a valid
    GTP response (see @ref GtpEngine::MainLoop).
*/

class GtpCommand
{
public:
    /** Construct empty command.
        @warning An empty command cannot be used, before Init() was called.
        This constructor exists only to reuse instances.
    */
    GtpCommand();

    /** Construct with a command line.
        @see Init()
    */
    GtpCommand(const std::string& line);

    /** Conversion to output stream.
        Returns reference to response stream.
    */
    operator std::ostream&();

    /** Get argument.
        @param number Argument index starting with 0
        @return Argument value
        @throws GtpFailure If no such argument
    */
    const std::string& Arg(std::size_t number) const;

    /** Get single argument.
        @return Argument value
        @throws GtpFailure If no such argument or command has more than one
        arguments
    */
    const std::string& Arg() const;

    /** Get argument converted to lowercase.
        @param number Argument index starting with 0
        @return Copy of argument value converted to lowercase
        @throws GtpFailure If no such argument
    */
    std::string ArgToLower(std::size_t number) const;

    /** Get integer argument converted boolean.
        @param number Argument index starting with 0
        @return false, if argument is 0, true, if 1
        @throws GtpFailure If no such argument, or argument has other value
    */
    bool BoolArg(std::size_t number) const;

    /** Check that command has no arguments.
        @throws GtpFailure If command has arguments
    */
    void CheckArgNone() const;

    /** Check number of arguments.
        @param number Expected number of arguments
        @throws GtpFailure If command has a different number of arguments
    */
    void CheckNuArg(std::size_t number) const;

    /** Check maximum number of arguments.
        @param number Expected maximum number of arguments
        @throws GtpFailure If command has more arguments
    */
    void CheckNuArgLessEqual(std::size_t number) const;

    /** Get argument converted to double.
        @param number Argument index starting with 0
        @return Argument value
        @throws GtpFailure If no such argument, or argument is not a double
    */
    double FloatArg(std::size_t number) const;

    /** Get command ID.
        @return ID or empty string, if command has no ID
    */
    std::string ID() const;

    /** Initialize with a command line.
        The line should be not empty, not contain only whitespaces and not
        be a comment line.
        It will be split into the optional numeric command ID, the command
        name, and arguments.
    */
    void Init(const std::string& line);

    /** Get argument converted to integer.
        @param number Argument index starting with 0
        @return Argument value
        @throws GtpFailure If no such argument, or argument is not an integer
    */
    int IntArg(std::size_t number) const;

    /** Get argument converted to integer in a range with lower limit.
        @param number Argument index starting with 0
        @param min Minimum allowed value
        @return Argument value
        @throws GtpFailure If no such argument, argument is not an integer,
        or not in range
    */
    int IntArg(std::size_t number, int min) const;

    /** Get argument converted to integer in a range with lower and upper
        limit.
        @param number Argument index starting with 0
        @param min Minimum allowed value
        @param max Maximum allowed value
        @return Argument value
        @throws GtpFailure If no such argument, argument is not an integer,
        or not in range
    */
    int IntArg(std::size_t number, int min, int max) const;

    /** Get argument line.
        Get all arguments as a line.
        No modfications to the line were made apart from trimmimg leading
        and trailing white spaces.
    */
    std::string ArgLine() const;

    /** Get command line.
        Returns full command line as given to the constructor or
        GtpCommand::Init.
        No modfications to the line were made apart from trimmimg leading
        and trailing white spaces.
    */
    const std::string& Line() const;

    /** Get argument name. */
    const std::string& Name() const;

    /** Get number of arguments. */
    std::size_t NuArg() const;

    /** Return remaining line after argument.
        @param number Argument index starting with 0
        @return The remaining line after the given argument, unmodified apart
        from leading and trailing whitespaces, which are trimmed. Quotation
        marks are not handled.
        @throws GtpFailure If no such argument
    */
    std::string RemainingLine(std::size_t number) const;

    /** Get response.
        @return A copy of the internal response string stream
    */
    std::string Response() const;

    /** Get internal response string stream */
    std::ostringstream& ResponseStream();

    /** Set response. */
    void SetResponse(const std::string& response);

    /** Set response to "true" or "false". */
    void SetResponseBool(bool value);

    /** Get argument converted to std::size_t.
        @param number Argument index starting with 0
        @return Argument value
        @throws GtpFailure If no such argument, or argument is not a size_t
    */
    std::size_t SizeTypeArg(std::size_t number) const;

    /** Get argument converted to std::size_t with lower limit.
        @param number Argument index starting with 0
        @param min Minimum allowed value
        @return Argument value
        @throws GtpFailure If no such argument, or argument is not a size_t
    */
    std::size_t SizeTypeArg(std::size_t number, std::size_t min) const;

private:
    /** Argument in command line. */
    struct Argument
    {
        /** Argument value.
            Enclosing quotes are removed if there were any and escape
            characters within enclosing quotes are removed.
        */
        std::string m_value;

        /** Position of first character in m_line after this argument. */
        std::size_t m_end;

        Argument(const std::string& value, std::size_t end);
    };

    /** Dummy stream for copying default formatting settings. */
    static std::ostringstream s_dummy;

    /** ID of command or empty string, if command has no ID */
    std::string m_id;

    /** Full command line. */
    std::string m_line;

    /** Response stream */
    std::ostringstream m_response;

    /** Arguments of command. */
    std::vector<Argument> m_arguments;

    void ParseCommandId();

    void SplitLine(const std::string& line);
};

/** @relates GtpCommand */
template<typename TYPE>
GtpCommand& operator<<(GtpCommand& cmd, const TYPE& type)
{
    cmd.ResponseStream() << type;
    return cmd;
}

/** @relates GtpCommand */
template<typename TYPE>
GtpCommand& operator<<(GtpCommand& cmd, TYPE& type)
{
    cmd.ResponseStream() << type;
    return cmd;
}

inline GtpCommand::GtpCommand()
{
}

inline GtpCommand::GtpCommand(const std::string& line)
{
    Init(line);
}

inline GtpCommand::operator std::ostream&()
{
    return ResponseStream();
}

inline void GtpCommand::CheckArgNone() const
{
    CheckNuArg(0);
}

inline std::string GtpCommand::ID() const
{
    return m_id;
}

inline const std::string& GtpCommand::Line() const
{
    return m_line;
}

inline const std::string& GtpCommand::Name() const
{
    return m_arguments[0].m_value;
}

inline std::size_t GtpCommand::NuArg() const
{
    return m_arguments.size() - 1;
}

inline std::string GtpCommand::Response() const
{
    return m_response.str();
}

inline std::ostringstream& GtpCommand::ResponseStream()
{
    return m_response;
}

//----------------------------------------------------------------------------

/** Abstract base class for command handlers. */
class GtpCallbackBase
{
public:
    virtual ~GtpCallbackBase() throw();

    virtual void operator()(GtpCommand&) = 0;
};

//----------------------------------------------------------------------------

/** Member function command handlers.
    For registering member functions in GtpEngine::Register().
    @note Instances keep a pointer to the object containing the member
    function. If the object does is not a subclass of GtpEngine and registers
    only its own members, you have to make sure that the object's lifetime
    exceeds the lifetime of the GtpEngine.
*/
template<class ENGINE>
class GtpCallback
    : public GtpCallbackBase
{
public:
    /** Signature of the member function. */
    typedef void (ENGINE::*Method)(GtpCommand&);

    GtpCallback(ENGINE* instance,
                typename GtpCallback<ENGINE>::Method method);

    ~GtpCallback() throw();

    /** Execute the member function. */
    void operator()(GtpCommand&);

private:
    ENGINE* m_instance;

    Method m_method;
};

template<class ENGINE>
GtpCallback<ENGINE>::GtpCallback(ENGINE* instance,
                                 typename GtpCallback<ENGINE>::Method method)
    : m_instance(instance),
      m_method(method)
{
}

template<class ENGINE>
GtpCallback<ENGINE>::~GtpCallback() throw()
{
#ifndef NDEBUG
    m_instance = 0;
#endif
}

template<class ENGINE>
void GtpCallback<ENGINE>::operator()(GtpCommand& cmd)
{
    (m_instance->*m_method)(cmd);
}

//----------------------------------------------------------------------------

/** @page gtpenginesimulatedelay Simulated Delays
    If the engine receives a special comment line
    <code># gtpengine-sleep n</code>, it will sleep for @c n seconds before
    reading the next line. This feature can be used for adding simulated
    delays in test GTP scripts (e.g. to test pondering functionality).
    Note that the start time for sleeping is at the start of the previous
    command, not when the response to the previous command is received,
    because GtpEngine continues reading the stream while the previous command
    is in progress. This functionality is only enabled, if GtpEngine was
    compiled with GTPENGINE_INTERRUPT.
*/

/** Base class for GTP (Go Text Protocol) engines.
    Commands can be added with GtpEngine::Register().
    Existing commands can be overridden by registering a new handler for
    the command or by overriding the command handler member function
    in subclasses.
    @see @ref gtpenginecommands, @ref gtpenginesimulatedelay
*/
class GtpEngine
{
public:
    /** @page gtpenginecommands GtpEngine Commands
        - @link CmdKnownCommand() @c known_command @endlink
        - @link CmdListCommands() @c list_commands @endlink
        - @link CmdName() @c name @endlink
        - @link CmdProtocolVersion() @c protocol_version @endlink
        - @link CmdQuit() @c quit @endlink
        - @link CmdVersion() @c version @endlink
    */
    /** @name Command Callbacks */
    // @{
    virtual void CmdKnownCommand(GtpCommand&);
    virtual void CmdListCommands(GtpCommand&);
    virtual void CmdName(GtpCommand&);
    virtual void CmdProtocolVersion(GtpCommand&);
    virtual void CmdQuit(GtpCommand&);
    virtual void CmdVersion(GtpCommand&);
    // @} // @name

    /** Constructor.
        @param in Input GTP stream
        @param out Output GTP stream
    */
    GtpEngine(GtpInputStream& in, GtpOutputStream& out);

    virtual ~GtpEngine();

    /** Execute commands from file.
        Aborts on the first command that fails.
        @param name The file name
        @param log Stream for logging the commands and responses to (default
        is std::cerr).
        @throw GtpFailure If a command fails
    */
    void ExecuteFile(const std::string& name, std::ostream& log = std::cerr);

    /** Execute a single command string.
        @param cmd The command line
        @param log Stream for logging the command and response to (default
        is std::cerr).
        @returns The command response
        @throw GtpFailure If the command fails
    */
    std::string ExecuteCommand(const std::string& cmd,
                               std::ostream& log = std::cerr);

    /** Run the main command loop.
        Reads lines from input stream, calls the corresponding command
        handler and writes the response to the output stream.
        Empty lines in the command responses will be replaced by a line
        containing a single space, because empty lines are not allowed
        in GTP responses.
    */
    void MainLoop();

    /** Register command handler.
        Takes ownership of callback.
        If a command was already registered with the same name,
        it will be replaced by the new command.
    */
    void Register(const std::string& name, GtpCallbackBase* callback);

    /** Register a member function as a command handler.
        If a command was already registered with the same name,
        it will be replaced by the new command.
    */
    template<class T>
    void Register(const std::string& command,
                  typename GtpCallback<T>::Method method, T* instance);

    /** Returns if command registered. */
    bool IsRegistered(const std::string& command) const;

    /** Set flag for quitting the main command loop.
        Currently, this function works only for the "quit" command, if the
        engine is compiled with interrupt functionality (GTENGINE_INTERRUPT).
        Therefore, it is not possible for other commands to decide to quit
        (which would be necessary for instance to implement a maximal game
        number if playing on KGS and deciding to quit on the kgs-game_over
        command, if the maximum number is reached).

        The reason is that the command  stream is then read from a different
        thread using a blocking, non-interruptible read function, which is
        entered before the command handler is invoked in the main thread.
        Because of the non-interruptible read function, the implementation of
        GtpEngine needs to know what commands will quit to avoid entering this
        read function after a quit.

        If a way is found to interrupt the read thread during the execution
        of the blocking std::getline (maybe in a future version of
        Boost.Thread), this function could also be called in other GTP
        commands.
        @see MainLoop()
    */
    void SetQuit();

    /** Did the last command set the quit flag? */
    bool IsQuitSet() const;

#if GTPENGINE_PONDER
    /** Ponder.
        This function will be called in MainLoop() while the engine is waiting
        for the next command.
        It will be called after InitPonder() from a different thread than
        the command thread, but only while waiting for the next command, so
        no concurrent execution of this function and other engine functions
        is possible. The function should return immediately when StopPonder()
        is called. InitPonder() and StopPonder() are called from the
        command thread.
        In a typical implementation, InitPonder() will clear an abort flag and
        StopPonder() will set it. Ponder() will poll the abort flag and return
        when it is set (or it has nothing to do; or some maximum time limit
        for pondering was exceeded).
        The default implementation does nothing and returns immediately.
    */
    virtual void Ponder();

    /** Prepare for pondering.
        @see Ponder()
        The default implementation does nothing.
    */
    virtual void InitPonder();

    /** Stop pondering.
        @see Ponder()
        The default implementation does nothing.
    */
    virtual void StopPonder();
#endif // GTPENGINE_PONDER

#if GTPENGINE_INTERRUPT
    /** Interrupt the current command.
        This function implements interrupt functionality as used by
        <a href="http://gogui.sf.net">GoGui</a>. It will be called from a
        different thread that the command thread when the special command
        line <tt># interrupt</tt> is received.
        The default implementation does nothing.
    */
    virtual void Interrupt();
#endif // GTPENGINE_INTERRUPT

protected:
    /** Hook function to be executed before each command.
        Default implementation does nothing.
    */
    virtual void BeforeHandleCommand();

    /** Hook function to be executed before the response of a command is
        written.
        Default implementation does nothing.
    */
    virtual void BeforeWritingResponse();

private:
    typedef std::map<std::string,GtpCallbackBase*> CallbackMap;

    bool m_quit;

    GtpInputStream& m_in;

    GtpOutputStream& m_out;

    CallbackMap m_callbacks;

    /** Not to be implemented. */
    GtpEngine(const GtpEngine& engine);

    /** Not to be implemented. */
    GtpEngine& operator=(const GtpEngine& engine) const;

    bool HandleCommand(GtpCommand& cmd, GtpOutputStream& out);
};

template<class T>
void GtpEngine::Register(const std::string& command,
                         typename GtpCallback<T>::Method method, T* instance)
{
    Register(command, new GtpCallback<T>(instance, method));
}

//----------------------------------------------------------------------------

#endif // GTPENGINE_H

