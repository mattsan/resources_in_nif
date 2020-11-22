#include <cstdio>
#include <new>
#include <erl_nif.h>
namespace {

// サンプルクラス
// 外部リソースの解放のタイミングをプログラマが指定できるように、
// リソース解放の destroy を destructor から分離している。
class MyApp {
public:
    MyApp(int n) : destroyed_(false), n_(n) {
        enif_fprintf(stdout, "MyApp constructed : %d\n", n_);
    }

    ~MyApp() {
        destroy();
        enif_fprintf(stdout, "MyApp destructed : %d\n", n_);
    }

    // 外部リソースを獲得する method
    void create() {
        enif_fprintf(stdout, "MyApp created : %d\n", n_);
    }

    // 外部リソースを解放する method
    // 多重で呼ばれてもリソースの解放を複数回行わないように解放状態を管理している
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
    // 外部リソースの解放状態
    bool destroyed_;

    // 任意のメンバ変数
    // ここでは MyApp.create/1 呼び出しの時に渡される整数値を格納する
    int n_;
};

// リソースの型
ErlNifResourceType* MyAppType;

// garbage collection でリソースが解放されるときに呼び出される関数
void destruct_my_app(ErlNifEnv*, void* obj) {
    MyApp* my_app = static_cast<MyApp*>(obj);

    // placement delete で destructor を呼び出す
    my_app->~MyApp();
}

// ライブラリのロード時に呼び出される関数
// ERL_NIF_INIT で指定する。
int load(ErlNifEnv* env, void**, ERL_NIF_TERM load_info) {
    enif_fprintf(stdout, "loading my_app_nif\n");

    // リソースの型を取得する
    // リソースを解放するときに呼び出される関数を指定する。
    MyAppType = enif_open_resource_type(env, "MyApp", "MyApp", destruct_my_app, ERL_NIF_RT_CREATE, NULL);

    return 0;
}

// MyApp.create/1 にマウントされる関数
// この例では、MyApp.create/1 の引数に渡された数値を保持するリソースを作成している
ERL_NIF_TERM create(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    // 第一引数の値を取得する
    int n;
    if (!enif_get_int(env, argv[0], &n)) {
        return enif_make_badarg(env);
    }

    // リソース領域を獲得する
    void* resource = enif_alloc_resource(MyAppType, sizeof(MyApp));

    // リソースハンドルを作成する
    // 所有権は Erlang に移動する
    // リソースの削除は garbage collection で行われる
    ERL_NIF_TERM result = enif_make_resource(env, resource);

    // 所有権を移譲したのでリソースの参照を解放する
    enif_release_resource(resource);

    // placement new で constructor を呼び出し、MyApp のオブジェクトを構築する
    MyApp* my_app = new(resource) MyApp(n);

    // 外部リソースを獲得する
    my_app->create();

    // リソースハンドルを返却する
    // Erlang/Elixir からは reference の値として見える
    return result;
}

// MyApp.do_something/1 にマウントされる関数
ERL_NIF_TERM do_something(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    // リソースハンドル (reference) から MyApp のオブジェクトを取得する
    MyApp* my_app;
    if (!enif_get_resource(env, argv[0], MyAppType, reinterpret_cast<void**>(&my_app))) {
        return enif_make_badarg(env);
    }

    my_app->do_something();

    return enif_make_atom(env, "ok");
}

ERL_NIF_TERM destroy(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    // リソースハンドル (reference) から MyApp のオブジェクトを取得する
    MyApp* my_app;
    MyApp* my_app;
    if (!enif_get_resource(env, argv[0], MyAppType, reinterpret_cast<void**>(&my_app))) {
        return enif_make_badarg(env);
    }

    // 外部リソースを解放する
    my_app->destroy();

    return enif_make_atom(env, "ok");
}

ErlNifFunc nif_funcs[] = {
    {"create", 1, create},
    {"do_something", 1, do_something},
    {"destroy", 1, destroy}
};

} // namespace

ERL_NIF_INIT(Elixir.MyApp, nif_funcs, load, NULL, NULL, NULL);
