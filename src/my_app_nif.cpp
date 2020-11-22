#include <cstdio>
#include <new>
#include <erl_nif.h>

class MyApp {
public:
    MyApp(int n) : destroyed_(false), n_(n) {
        enif_fprintf(stdout, "MyApp constructed : %d\n", n_);
    }

    ~MyApp() {
        destroy();
        enif_fprintf(stdout, "MyApp destructed : %d\n", n_);
    }

    void create() {
        enif_fprintf(stdout, "MyApp created : %d\n", n_);
    }

    void destroy() {
        if (!destroyed_) {
            destroyed_ = true;
            enif_fprintf(stdout, "MyApp destroyed : %d\n", n_);
        }
    }

    void do_something() {
        enif_fprintf(stdout, "do something : %d\n", n_);
    }
    
private:
    bool destroyed_;
    int n_;
};

ErlNifResourceType* MyAppType;

void destruct_my_app(ErlNifEnv*, void* obj) {
    MyApp* my_app = static_cast<MyApp*>(obj);
    my_app->~MyApp();
}

int load(ErlNifEnv* env, void**, ERL_NIF_TERM load_info) {
    MyAppType = enif_open_resource_type(env, "MyApp", "MyApp", destruct_my_app, ERL_NIF_RT_CREATE, NULL);
    return 0;
}

ERL_NIF_TERM create(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    int n;
    if (!enif_get_int(env, argv[0], &n)) {
        return enif_make_badarg(env);
    }

    void* resource = enif_alloc_resource(MyAppType, sizeof(MyApp));
    ERL_NIF_TERM result = enif_make_resource(env, resource);
    enif_release_resource(resource);

    MyApp* my_app = new(resource) MyApp(n);
    my_app->create();

    return result;
}

ERL_NIF_TERM do_something(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    MyApp* my_app;
    if (!enif_get_resource(env, argv[0], MyAppType, reinterpret_cast<void**>(&my_app))) {
        return enif_make_badarg(env);
    }

    my_app->do_something();

    return enif_make_atom(env, "ok");
}

ERL_NIF_TERM destroy(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    MyApp* my_app;
    if (!enif_get_resource(env, argv[0], MyAppType, reinterpret_cast<void**>(&my_app))) {
        return enif_make_badarg(env);
    }

    my_app->destroy();

    return enif_make_atom(env, "ok");
}

ErlNifFunc nif_funcs[] = {
    {"create", 1, create},
    {"do_something", 1, do_something},
    {"destroy", 1, destroy}
};

ERL_NIF_INIT(Elixir.MyApp, nif_funcs, load, NULL, NULL, NULL);
