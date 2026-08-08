{
  description = "verti's gtk4 desktop bar";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      systems = [ "x86_64-linux" ];
      forAllSystems = f: builtins.listToAttrs (map (s: { name = s; value = f s; }) systems);
      pkgsFor = system: nixpkgs.legacyPackages.${system};
    in {
      packages = forAllSystems (system:
        let pkgs = pkgsFor system; in {
          default = pkgs.stdenv.mkDerivation {
            pname = "vbar";
            version = "0.0.1";
            src = ./.;
            nativeBuildInputs = with pkgs; [ cmake pkg-config ];
            buildInputs = with pkgs; [ gtkmm4 gtk4-layer-shell wireplumber ];
          };
        }
      );

      devShells = forAllSystems (system:
        let pkgs = pkgsFor system; in {
          default = pkgs.mkShell {
            buildInputs = with pkgs; [ gcc cmake just pkg-config gtkmm4 gtk4-layer-shell wireplumber ];
            shellHook = ''
              exec fish
            '';
          };
        }
      );

      homeManagerModules.default = { config, pkgs, lib, ... }:
        let
          cfg = config.programs.vbar;
        in {
          options.programs.vbar = {
            enable = lib.mkEnableOption "verti's gtk4 desktop bar";
            package = lib.mkOption {
              type = lib.types.package;
              default = self.packages.${pkgs.system}.default;
              description = "package to use";
            };
            settings = lib.mkOption {
              type = lib.types.submodule {
                options = {
                  theme = lib.mkOption {
                    type = lib.types.enum [
                      "catppuccin mocha" "catppuccin macchiato"
                      "catppuccin frappe" "catppuccin latte"
                    ];
                    default = "catppuccin mocha";
                    description = "Color theme.";
                  };
                  bar = {
                    position = lib.mkOption {
                      type = lib.types.enum [ "top" "bottom" "left" "right" ];
                      default = "top";
                    };
                    thickness = lib.mkOption {
                      type = lib.types.ints.unsigned;
                      default = 20;
                    };
                    margin-left = lib.mkOption {
                      type = lib.types.ints.unsigned;
                      default = 0;
                    };
                    margin-right = lib.mkOption {
                      type = lib.types.ints.unsigned;
                      default = 0;
                    };
                    margin-top = lib.mkOption {
                      type = lib.types.ints.unsigned;
                      default = 0;
                    };
                    margin-bottom = lib.mkOption {
                      type = lib.types.ints.unsigned;
                      default = 0;
                    };
                  };
                };
              };
              default = {};
            };
          };

          config = lib.mkIf cfg.enable {
            home.packages = [ cfg.package ];
            home.file.".vbar.ini".text = ''
              theme = ${cfg.settings.theme}
              [bar]
              position = ${cfg.settings.bar.position}
              thickness = ${toString cfg.settings.bar.thickness}
              ${lib.optionalString (cfg.settings.bar.margin-left != 0) "margin-left = ${toString cfg.settings.bar.margin-left}"}
              ${lib.optionalString (cfg.settings.bar.margin-right != 0) "margin-right = ${toString cfg.settings.bar.margin-right}"}
              ${lib.optionalString (cfg.settings.bar.margin-top != 0) "margin-top = ${toString cfg.settings.bar.margin-top}"}
              ${lib.optionalString (cfg.settings.bar.margin-bottom != 0) "margin-bottom = ${toString cfg.settings.bar.margin-bottom}"}
            '';
          };
        };
    };
}
