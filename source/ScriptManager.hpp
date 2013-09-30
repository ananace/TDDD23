#pragma once

#include <vector>
#include <cassert>

class asIScriptContext;
class asIScriptEngine;

#define asAssert(r) assert((r) >= 0)

/** A basic manager to keep track of scripts
 */
class ScriptManager
{
public:
    ScriptManager();
    ~ScriptManager();

    void init();

    asIScriptContext* getContext();
    void returnContext(asIScriptContext* ctx);

    void runString(const std::string& code);

private:
    /// Called by the script engine when a context fails execution.
    void exceptionCallback(asIScriptContext*);

    asIScriptEngine* mEngine;
    std::vector<asIScriptContext*> mContexts;
};
