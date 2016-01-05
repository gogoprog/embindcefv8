#pragma once

#include <cstring>

#ifdef EMSCRIPTEN
    #include <emscripten.h>
    #include <emscripten/bind.h>
#else
    #include "include/cef_client.h"
    #include "include/cef_app.h"
    #include <functional>
#endif

namespace embindcefv8
{
    #ifdef CEF
        typedef std::function<void(CefRefPtr<CefV8Value>&)>
            Registerer;
        typedef std::function<void(CefRefPtr<CefV8Value>&)>
            ResultFunction;
        typedef std::function<void(CefRefPtr<CefV8Value>&, void*)>
            GetterFunction;

        void onContextCreated(CefV8Context* context);
        std::vector<Registerer> & getRegisterers();

        class FuncHandler : public CefV8Handler
        {
        public:
            FuncHandler(ResultFunction & _func) : CefV8Handler()
            {
                func = _func;
            }

            virtual bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) override
            {
                func(retval);
                return true;
            }

            IMPLEMENT_REFCOUNTING(FuncHandler);
        private:
            ResultFunction
                func;
        };

        template<typename T>
        struct ValueCreator;

        template<>
        struct ValueCreator<float>
        {
            static void create(CefRefPtr<CefV8Value>& retval, float & value)
            {
                retval = CefV8Value::CreateDouble(value);
            }
        };

        template<>
        struct ValueCreator<int>
        {
            static void create(CefRefPtr<CefV8Value>& retval, int & value)
            {
                retval = CefV8Value::CreateInt(value);
            }
        };

        template<>
        struct ValueCreator<std::string>
        {
            static void create(CefRefPtr<CefV8Value>& retval, std::string & value)
            {
                retval = CefV8Value::CreateString(value);
            }
        };
    #endif

    template<class T>
    class ValueObject
    {
    public:
        ValueObject(const char *_name)
        {
            name = _name;

            #ifdef EMSCRIPTEN
                vo = new emscripten::value_object<T>(_name);
            #else
                registerConstructor = false;
            #endif
        }

        ~ValueObject()
        {
            #ifdef EMSCRIPTEN
                delete vo;
            #else
                if(registerConstructor)
                {
                    auto copied_name = name;
                    auto copied_getters = getters;

                    getRegisterers().push_back(
                            [copied_name, copied_getters](CefRefPtr<CefV8Value> & module_object)
                            {
                                ResultFunction fc = [copied_getters](CefRefPtr<CefV8Value>& retval) {
                                    T new_object;
                                    ValueCreator<T>::create(retval, new_object);
                                };

                                CefRefPtr<CefV8Value> constructor_func = CefV8Value::CreateFunction(copied_name.c_str(), new FuncHandler(fc));
                                module_object->SetValue(copied_name.c_str(), constructor_func, V8_PROPERTY_ATTRIBUTE_NONE);
                            }
                        );
                }
            #endif
        }

        ValueObject & constructor()
        {
            #ifdef EMSCRIPTEN
                T ( * func )() =
                    []() -> T
                    {
                        return T();
                    };

                emscripten::function(name.c_str(), func);
            #else
                registerConstructor = true;
            #endif

            return *this;
        }

        template<class F>
        ValueObject & field(const char *fieldName, F (T::*field))
        {
            #ifdef EMSCRIPTEN
                vo->field(fieldName, field);
            #else
                getters[fieldName] = [field](CefRefPtr<CefV8Value>& retval, void * object) {
                    ValueCreator<F>::create(retval, (*(T *)object).*field);
                };
            #endif

            return *this;
        }

        template<typename C>
        friend class ValueCreator;

    private:
        std::string
            name;
        #ifdef EMSCRIPTEN
            emscripten::value_object<T>
                * vo;
        #else
            static std::map<std::string, GetterFunction>
                getters;
            bool
                registerConstructor;
        #endif
    };

    #ifdef CEF
        template<class T>
        std::map<std::string, GetterFunction> ValueObject<T>::getters;

        template<typename T>
        struct ValueCreator
        {
            static void create(CefRefPtr<CefV8Value>& retval, T& value)
            {
                retval = CefV8Value::CreateObject(nullptr);

                for(auto& kv : ValueObject<T>::getters)
                {
                    CefRefPtr<CefV8Value> field_value;
                    kv.second(field_value, (void*) &value);
                    retval->SetValue(kv.first, field_value, V8_PROPERTY_ATTRIBUTE_NONE);
                }
            }
        };
    #endif

    void execute(const char *str);
}
