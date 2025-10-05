import pathlib, subprocess, os, shutil, tomllib, zipfile, sys, json, platform
from pathlib import Path


class ModInfo:
    project_root: Path
    mod_toml_file: Path
    mod_data: dict
    
    def __init__(self, mod_toml_str: str, build_dir: str):
        self.project_root = Path(__file__).parent
        self.mod_toml_file = self.project_root.joinpath(mod_toml_str)
        
        self.mod_data = tomllib.loads(self.mod_toml_file.read_text())
        # print(mod_data)
        self.build_dir = self.project_root.joinpath(build_dir)
        self.build_nrm_file = self.build_dir.joinpath(f"{self.mod_data['inputs']['mod_filename']}.nrm")
        
        self.runtime_dir = self.project_root.joinpath("runtime")
        self.runtime_mods_dir = self.runtime_dir.joinpath("mods")
        self.runtime_nrm_file = self.runtime_mods_dir.joinpath(f"{self.mod_data['inputs']['mod_filename']}.nrm")
        
        self.assets_archive_path =self.project_root.joinpath("assets_archive.zip")
        
        # Handle recomp compilers:
        self.user_config_path = self.project_root.joinpath("./user_build_config.json")
        self.user_config = {}
        if not self.user_config_path.exists():
            self.create_user_build_config()
        
        else:
            self.user_config = json.loads(self.user_config_path.read_text())

        self.build_dll_file: Path = None
        self.build_pdb_file: Path = None
        self.build_dylib_file: Path = None
        self.build_so_file: Path = None
        self.build_native_file: Path = None
        self.build_native_pdb_file: Path = None
        
        self.runtime_dll_file: Path = None
        self.runtime_pdb_file: Path = None
        self.runtime_dylib_file: Path = None
        self.runtime_so_file: Path = None
        self.runtime_native_file: Path = None
        self.runtime_native_pdb_file: Path = None
        
        
        
    def set_extlib_info(self, windows_lib: str, macos_lib: str, linux_lib: str, native_lib: str):
        self.build_dll_file = self.project_root.joinpath(windows_lib)
        self.build_pdb_file = self.build_dll_file.with_suffix(".pdb")
        self.build_dylib_file = self.project_root.joinpath(macos_lib)
        self.build_so_file = self.project_root.joinpath(linux_lib)
        self.build_native_file = self.project_root.joinpath(native_lib)
        self.build_native_pdb_file = self.build_native_file.with_suffix(".pdb")
        
        self.runtime_dll_file = self.runtime_mods_dir.joinpath(self.build_dll_file.name.removeprefix("lib"))
        self.runtime_pdb_file = self.runtime_mods_dir.joinpath(self.build_pdb_file.name.removeprefix("lib"))
        self.runtime_dylib_file = self.runtime_mods_dir.joinpath(self.build_dylib_file.name.removeprefix("lib"))
        self.runtime_so_file = self.runtime_mods_dir.joinpath(self.build_so_file.name.removeprefix("lib"))
        self.runtime_native_file = self.runtime_mods_dir.joinpath(self.build_native_file.name.removeprefix("lib"))
        self.runtime_native_pdb_file = self.runtime_mods_dir.joinpath(self.build_native_pdb_file.name.removeprefix("lib"))
        
        return self
    
    def create_user_build_config(self):
        self.user_config = {
            "mod_compiling": {
                "compiler": "clang",
                "linker": "ld.lld"
            },
        }
        
        if 'extlib_compiling' in self.mod_data:
            self.user_config["extlib_compiling"] = {
                "preset_groups": {
                    "Debug": {
                        "windows": {
                            "configure": self.mod_data['extlib_compiling']['windows_debug_configure_preset'],
                            "build": self.mod_data['extlib_compiling']['windows_debug_build_preset']
                        },
                        "macos": {
                            "configure": self.mod_data['extlib_compiling']['macos_debug_configure_preset'],
                            "build": self.mod_data['extlib_compiling']['macos_debug_build_preset']
                        },
                        "linux": {
                            "configure": self.mod_data['extlib_compiling']['linux_debug_configure_preset'],
                            "build": self.mod_data['extlib_compiling']['linux_debug_build_preset']
                        },
                        "native": {
                            "configure": self.get_native_preset("Debug"),
                            "build": self.get_native_preset("Debug")
                        }
                    },
                    "Release": {
                        "windows": {
                            "configure": self.mod_data['extlib_compiling']['windows_release_configure_preset'],
                            "build": self.mod_data['extlib_compiling']['windows_release_build_preset']
                        },
                        "macos": {
                            "configure": self.mod_data['extlib_compiling']['macos_release_configure_preset'],
                            "build": self.mod_data['extlib_compiling']['macos_release_build_preset']
                        },
                        "linux": {
                            "configure": self.mod_data['extlib_compiling']['linux_release_configure_preset'],
                            "build": self.mod_data['extlib_compiling']['linux_release_build_preset']
                        },
                        "native": {
                            "configure": self.get_native_preset("Release"),
                            "build": self.get_native_preset("Release")
                        }
                    }
                }
            }
            
        self.user_config_path.write_text(json.dumps(self.user_config, indent=4))
    
    def get_native_preset(self, build_type: str):
        if platform.system() == "Windows":
            return f"native-windows-x64-{build_type}"
        elif platform.system() == "Darwin":
            return f"native-macos-aarch64-{build_type}"
        else:
            return f"native-linux-x64-{build_type}"
        
    def get_mod_file(self):
        name = f"{self.mod_data['inputs']['mod_filename']}.nrm"
        return self.print_and_return(self.build_dir.joinpath(name))
    
    def get_mod_elf(self):
        return self.print_and_return(self.mod_toml_file.parent.joinpath(self.mod_data['inputs']['elf_path']))
        
    def get_mod_compiler(self):
        return self.print_and_return(self.user_config["mod_compiling"]["compiler"])
        
    def get_mod_linker(self):
        return self.print_and_return(self.user_config["mod_compiling"]["linker"])
        
    def get_extlib_name(self):
        if 'extlib_compiling' in self.mod_data:
            return self.print_and_return(self.mod_data['extlib_compiling']['library_name'])
        else:
            return self.print_and_return(None)

    def get_extlib_windows_configure_preset(self, build_type: str):
        if 'extlib_compiling' not in self.mod_data: 
            return None
        return self.print_and_return(self.user_config["extlib_compiling"]["preset_groups"][build_type]["windows"]["configure"])

    def get_extlib_macos_configure_preset(self, build_type: str):
        if 'extlib_compiling' not in self.mod_data: 
            return None
        return self.print_and_return(self.user_config["extlib_compiling"]["preset_groups"][build_type]["macos"]["configure"])

    def get_extlib_linux_configure_preset(self, build_type: str):
        if 'extlib_compiling' not in self.mod_data: 
            return None
        return self.print_and_return(self.user_config["extlib_compiling"]["preset_groups"][build_type]["linux"]["configure"])

    def get_extlib_native_configure_preset(self, build_type: str):
        if 'extlib_compiling' not in self.mod_data: 
            return None
        return self.print_and_return(self.user_config["extlib_compiling"]["preset_groups"][build_type]["native"]["configure"])

    def get_extlib_windows_build_preset(self, build_type: str):
        if 'extlib_compiling' not in self.mod_data: 
            return None
        return self.print_and_return(self.user_config["extlib_compiling"]["preset_groups"][build_type]["windows"]["build"])

    def get_extlib_macos_build_preset(self, build_type: str):
        if 'extlib_compiling' not in self.mod_data: 
            return None
        return self.print_and_return(self.user_config["extlib_compiling"]["preset_groups"][build_type]["macos"]["build"])

    def get_extlib_linux_build_preset(self, build_type: str):
        if 'extlib_compiling' not in self.mod_data: 
            return None
        return self.print_and_return(self.user_config["extlib_compiling"]["preset_groups"][build_type]["linux"]["build"])

    def get_extlib_native_build_preset(self, build_type: str):
        if 'extlib_compiling' not in self.mod_data: 
            return None
        return self.print_and_return(self.user_config["extlib_compiling"]["preset_groups"][build_type]["native"]["build"])

    def create_asset_archive(self, assets_extract_path_str: str):
            assets_extract_path = self.project_root.joinpath(assets_extract_path_str)
            print(f"Assets folder '{assets_extract_path.name}' not found. Extracting assets from '{self.assets_archive_path.name}'...")
            with zipfile.ZipFile(self.assets_archive_path, 'r') as zip_ref:
                zip_ref.extractall(assets_extract_path)

    def copy_to_runtime_dir(self):
        # Copying files for debugging:
        os.makedirs(self.runtime_mods_dir, exist_ok=True)
        portable_txt = self.runtime_dir.joinpath("portable.txt")
        if not portable_txt.exists():
            portable_txt.write_text("")
            print(f"Created '{portable_txt}'.")
        
        self.copy_if_exists(self.build_nrm_file, self.runtime_nrm_file)
        # If no extlib is being built, we don't need to try to find these.
        if 'extlib_compiling' in self.mod_data:
            self.copy_if_exists(self.build_dll_file, self.runtime_dll_file)
            self.copy_if_exists(self.build_pdb_file, self.runtime_pdb_file)
            self.copy_if_exists(self.build_dylib_file, self.runtime_dylib_file)
            self.copy_if_exists(self.build_so_file, self.runtime_so_file)

    def copy_to_runtime_dir_native(self):
        # Copying files for debugging:
        os.makedirs(self.runtime_mods_dir, exist_ok=True)
        portable_txt = self.runtime_dir.joinpath("portable.txt")
        if not portable_txt.exists():
            portable_txt.write_text("")
            print(f"Created '{portable_txt}'.")
        
        self.copy_if_exists(self.build_nrm_file, self.runtime_nrm_file)
        # If no extlib is being built, we don't need to try to find these.
        if 'extlib_compiling' in self.mod_data:
            self.copy_if_exists(self.build_native_file, self.runtime_native_file)
            self.copy_if_exists(self.build_native_pdb_file, self.runtime_native_pdb_file)

    def copy_if_exists(self, src: Path, dest: Path):
        if src.exists():
            shutil.copy(src, dest)
            print(f"'{src}' copied to '{dest}'.")
        else:
            print(f"'{src}' does not exist. Skipping.")

    def run_clean(self):
        shutil.rmtree(self.build_dir)
        shutil.rmtree(self.project_root.joinpath("./N64Recomp/build"))

    def print_and_return(self, x):
        print(x)
        return x

def run_build(args: list[str]):
    make_run = subprocess.run(
        [
            shutil.which("make"),
        ] + args,
        cwd=pathlib.Path(__file__).parent
        
    )
    if make_run.returncode != 0:
        raise RuntimeError("Make failed!")

if __name__ == '__main__':
    run_build(sys.argv[1:])
