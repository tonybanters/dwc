{
  description = "DWC - A dynamic Wayland compositor";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            # Build tools
            zig
            pkg-config

            # Wayland dependencies
            wayland
            wayland-protocols
            wayland-scanner

            # wlroots and related libraries
            wlroots
            libxkbcommon

            # Additional libraries that might be needed
            pixman
            libGL
            libinput
            mesa
          ];

          shellHook = ''
            echo "DWC development environment loaded!"
            echo "Available commands:"
            echo "  zig build        - Build the compositor"
            echo "  zig build run    - Build and run the compositor"
            echo "  zig build test   - Test the compositor"
            echo "  zig build clean  - Clean build artifacts"
            echo ""
            echo "Note: To run the compositor, you may need to run it from a TTY or nested Wayland session."
          '';
        };

        packages.default = pkgs.stdenv.mkDerivation {
          pname = "dwc";
          version = "0.1.0";

          src = ./.;

          nativeBuildInputs = with pkgs; [
            zig
            pkg-config
            wayland-scanner
          ];

          buildInputs = with pkgs; [
            wayland
            wayland-protocols
            wlroots
            libxkbcommon
            pixman
            libGL
            libinput
            mesa
          ];

          buildPhase = ''
            zig build -Doptimize=ReleaseSafe
          '';

          installPhase = ''
            mkdir -p $out/bin
            cp zig-out/bin/dwc $out/bin/
          '';

          meta = with pkgs.lib; {
            description = "A dynamic Wayland compositor";
            license = licenses.gpl3;
            platforms = platforms.linux;
          };
        };
      }
    );
}
