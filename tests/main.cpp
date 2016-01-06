#include "embindcefv8.h"
#include <iostream>
#include "cef.h"

#ifdef EMSCRIPTEN
    #define EXECUTE_JS      EM_ASM
#else
    #define EXECUTE_JS(src) \
        executeJs( #src "\nstop();" );
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
        aMember()
    {
    }

    void aMethod()
    {
        aMember.floatMember *= 2;
        aMember.intMember *= 2;
        aMember.stringMember = "Another string";
    }

    AStruct
        aMember;
};

int main(int argc, char* argv[])
{
    {
        embindcefv8::ValueObject<AStruct>("AStruct")
            .constructor()
            .member("floatMember", &AStruct::floatMember)
            .member("intMember", &AStruct::intMember)
            .member("stringMember", &AStruct::stringMember)
            ;

        embindcefv8::Class<AStructContainer>("AStructContainer")
            .constructor()
            .member("aMember", &AStructContainer::aMember)
            .method("aMethod", &AStructContainer::aMethod)
            ;
    }

    #ifdef CEF
        initCef(argc, argv);
    #endif

    std::cout << "[embindcefv8] Begin tests" << std::endl;
    std::cout << "-------------------------" << std::endl;

    EXECUTE_JS(
        var test = Module.AStruct();
        console.log(test.floatMember);
        console.log(test.intMember);
        console.log(test.stringMember);

        test = Module.AStructContainer();
        console.log(test.aMember.floatMember);
        console.log(test.aMember.intMember);
        console.log(test.aMember.stringMember);

        test.aMethod();

        console.log(test.aMember.floatMember);
        console.log(test.aMember.intMember);
        console.log(test.aMember.stringMember);
    );


    #ifdef CEF
        finalizeCef();
    #endif

    std::cout << "-------------------------" << std::endl;
    std::cout << "[embindcefv8] End tests" << std::endl;

    return 0;
}
