defmodule MyApp do
  @moduledoc """
  NIF で C++ のクラスを扱うサンプル
  """

  # モジュールがロードされた時に NIF のライブラリをロードするためのフックを設定
  # see https://hexdocs.pm/elixir/Module.html#module-on_load
  @on_load {:load_nif, 0}

  # コンパイル時に NIF のライブラリがロードされないようにする設定
  # see https://hexdocs.pm/elixir/Module.html#module-compile
  @compile {:autoload, false}

  @doc """
  NIF をロードする

  :erlang.load_nif/2 は Erlang の文字列 = Elixir の文字リストを引数に取るため、
  to_charlist/1 で文字リストに変換している。
  """
  def load_nif do
    Application.app_dir(:my_app, "priv/my_app_nif")
    |> to_charlist()
    |> :erlang.load_nif(0)
  end

  def create(n) when is_integer(n) and n > 0, do: :erlang.nif_error(:nif_not_loaded)

  def do_something(ref) when is_reference(ref), do: :erlang.nif_error(:nif_not_loaded)

  def destroy(ref) when is_reference(ref), do: :erlang.nif_error(:nif_not_loaded)
end
