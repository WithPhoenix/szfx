{
  description = "Env for benchmarks";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };


  outputs = { self, nixpkgs }: let

    system = "x86_64-linux";
    pkgs = import nixpkgs { inherit system; };

    pythonEnv = pkgs.python3.withPackages (pk: with pk; [
      numpy
      pandas
      matplotlib
    ]);

    in {
      devShells.${system}.default = pkgs.mkShell {
        buildInputs = with pkgs; [
          gcc
          cmake
          gnumake
          stdenv.cc.cc.lib

          pythonEnv

          eigen
          eigen_5
          openblas
          lapack-reference

          pkg-config
        ];
        shellHook = ''
          # export LD_LIBRARY_PATH=${pkgs.stdenv.cc.cc.lib}/lib:$LD_LIBRARY_PATH
          export NIX_ENFORCE_NO_NATIVE=0
          echo "using native"

          # export CPATH="${pkgs.eigen}/include/eigen3:$CPATH"
          export CPATH="${pkgs.eigen}/include/eigen3:${pkgs.lapack-reference}/include:${pkgs.openblas}/include:$CPATH"
          export LIBRARY_PATH="${pkgs.lapack-reference}/lib:${pkgs.openblas}/lib:$LIBRARY_PATH"
          export LD_LIBRARY_PATH="${pkgs.stdenv.cc.cc.lib}/lib:${pkgs.lapack-reference}/lib:${pkgs.openblas}/lib:$LD_LIBRARY_PATH"
          
          echo "loaded c++ & benchmark env"
          echo "python $(python --version)"
          echo "gcc $(g++ --version | head -n 1)"
        '';
      };
    };
}
