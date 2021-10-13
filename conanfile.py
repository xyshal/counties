from conans import ConanFile
import platform

# Note: This is just a dependencies grabber, which needs to be a python class
# instead of a conanfile.txt because of dependency issues per-platform.

class QCounties(ConanFile):
    requires = "qt/6.1.3"
    generators = "cmake", "cmake_paths", "cmake_find_package", "virtualenv"

    def configure(self):
        self.options["qt"].shared = True
        self.options["qt"].qtsvg = True

        if platform.system() == "Windows":
            self.generators.append("virtualbuildenv")

    def build_requirements(self):
        if platform.system() == "Darwin":
            self.build_requires("cmake/[>=3.5]")

    def requirements(self):
        if platform.system() == "Darwin":
            self.requires("openssl/1.1.1l")
            self.requires("qt/6.1.3")
        else:
            self.requires("cmake/[>=3.5]")
