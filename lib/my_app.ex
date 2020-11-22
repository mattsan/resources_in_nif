defmodule MyApp do
  @on_load {:load_nif, 0}
  @compile {:autoload, false}

  def load_nif do
    Application.app_dir(:my_app, "priv/my_app_nif")
    |> to_charlist()
    |> :erlang.load_nif(0)
  end

  def create(n) when is_integer(n) and n > 0, do: :erlang.nif_error(:nif_not_loaded)
  def do_something(ref) when is_reference(ref), do: :erlang.nif_error(:nif_not_loaded)
  def destroy(ref) when is_reference(ref), do: :erlang.nif_error(:nif_not_loaded)
end
