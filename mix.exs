defmodule MyApp.MixProject do
  use Mix.Project

  def project do
    [
      app: :my_app,
      version: "0.1.0",
      elixir: "~> 1.11",
      start_permanent: Mix.env() == :prod,
      deps: deps(),
      compilers: [:elixir_make | Mix.compilers()],
      package: package()
    ]
  end

  def application do
    [
      extra_applications: [:logger]
    ]
  end

  defp deps do
    [
      {:elixir_make, "~> 0.6", runtime: false}
    ]
  end

  def package do
    [
      files: [
        "lib",
        "mix.exs",
        "README.md",
        "src/*.cpp",
        "src/*.h",
        "Makefile"
      ]
    ]
  end
end
