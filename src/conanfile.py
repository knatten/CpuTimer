import os

from conan import ConanFile
from conan.tools.files import copy


class CpuTimerRecipe(ConanFile):
    name = "cputimer"
    version = "1.0.0"
    license = "GPL v.3"
    author = "Anders Schau Knatten anders@knatten.org"
    url = "https://github.com/knatten/CpuTimer/"
    description = "Timers for real/process/thread time"
    exports_sources = "include/*"
    generators = "CMakeDeps"
    settings = "build_type"

    def requirements(self):
        self.requires("catch2/3.3.1")
        self.requires("benchmark/1.7.1")
        self.requires("fmt/9.1.0")

    def package(self):
        copy(self, "*.h", os.path.join(self.source_folder, "include"), os.path.join(self.package_folder, "include"))
