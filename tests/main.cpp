#include "embindcefv8.h"
#include <iostream>
#include "cef.h"

#ifdef EMSCRIPTEN
    #define executeFile(file) \
        EM_ASM( global.Module = Module; require('./main.js'); )
#endif

struct AStruct
{
    AStruct()
        :
        floatMember(16.0f),
        intMember(1024),
        stringMember("A sample string")
    {
    }

    AStruct(const int i)
        :
        floatMember(16.0f * i),
        intMember(1024 * i),
        stringMember("A sample string")
    {
    }

    float
        floatMember;
    int
        intMember;
    std::string
        stringMember;
};

struct AStructContainer
{
    AStructContainer()
        :
        aMember(),
        aInt(128)
    {
    }

    AStructContainer(const int a)
        :
        aMember(),
        aInt(a)
    {
    }

    void modifyMembers()
    {
        aMember.floatMember *= 2;
        aMember.intMember *= 2;
        aMember.stringMember = "Another string";
    }

    void aMethod()
    {
    }

    void aMethod1(const int a)
    {
    }

    void aMethod2(const int a, const int b)
    {
    }

    void aMethod3(const int a, const int b, const int c)
    {
    }

    int resultMethod()
    {
        return 128;
    }

    int resultMethod1(const int a)
    {
        return a;
    }

    int resultMethod2(const int a, const int b)
    {
        return a * b;
    }

    int resultMethod3(const int a, const int b, const int c)
    {
        return a * b * c;
    }

    AStruct constructAStruct(const int v)
    {
        return AStruct(v);
    }

    static int staticFunction()
    {
        return 32;
    }

    static int staticFunction1(int a)
    {
        return a;
    }

    static int staticFunction2(int a, int b)
    {
        return a + b;
    }

    static int staticFunction3(int a, int b, int c)
    {
        return a + b + c;
    }

    static int staticFunction4(int a, int b, int c, int d)
    {
        return a + b + c + d;
    }

    static int staticFunction5(int a, int b, int c, int d, int e)
    {
        return a + b + c + d + e;
    }

    AStruct
        aMember;
    int
        aInt;
};

class ADerivedClass : public AStructContainer
{
public:
    ADerivedClass() = default;

    int resultMethod()
    {
        return 256;
    }
};

EMBINDCEFV8_DECLARE_CLASS(AStructContainer, void)
EMBINDCEFV8_DECLARE_CLASS(ADerivedClass, AStructContainer)
EMBINDCEFV8_DECLARE_VALUE_OBJECT(AStruct)

EMBINDCEFV8_BINDINGS(test)
{
    embindcefv8::ValueObject<AStruct>("AStruct")
        .constructor()
        .constructor<int>()
        .property("floatMember", &AStruct::floatMember)
        .property("intMember", &AStruct::intMember)
        .property("stringMember", &AStruct::stringMember)
        ;

    embindcefv8::Class<AStructContainer>("AStructContainer")
        .constructor()
        .constructor<int>()
        .property("aMember", &AStructContainer::aMember)
        .property("aInt", &AStructContainer::aInt)
        .method("aMethod", &AStructContainer::aMethod)
        .method("aMethod1", &AStructContainer::aMethod1)
        .method("aMethod2", &AStructContainer::aMethod2)
        .method("aMethod3", &AStructContainer::aMethod3)
        .method("modifyMembers", &AStructContainer::modifyMembers)
        .method("resultMethod", &AStructContainer::resultMethod)
        .method("resultMethod1", &AStructContainer::resultMethod1)
        .method("resultMethod2", &AStructContainer::resultMethod2)
        .method("resultMethod3", &AStructContainer::resultMethod3)
        .method("constructAStruct", &AStructContainer::constructAStruct)
        .static_function("staticFunction", &AStructContainer::staticFunction)
        .static_function("staticFunction1", &AStructContainer::staticFunction1)
        .static_function("staticFunction2", &AStructContainer::staticFunction2)
        .static_function("staticFunction3", &AStructContainer::staticFunction3)
        .static_function("staticFunction4", &AStructContainer::staticFunction4)
        .static_function("staticFunction5", &AStructContainer::staticFunction5)
        ;

    embindcefv8::Class<ADerivedClass>("ADerivedClass")
        .constructor()
        .method("resultMethod", &ADerivedClass::resultMethod)
        ;
}

int main(int argc, char* argv[])
{
    #ifdef EMSCRIPTEN
        EM_ASM( global.Module = Module; var test = require('./kludjs.js'); global.test = test; require('./main.js'); );
    #else
        executeFile("kludjs.js");
        executeFile("main.js");
    #endif

    #ifdef CEF
        initCef(argc, argv);
        processLoop();
        finalizeCef();
    #endif

    return 0;
}
