from conans import ConanFile, CMake, tools


class MsqlitecppConan(ConanFile):
    name = "msqlitecpp"
    version = "1.99.5"
    license = "BSD3"
    author = "Federico Fuga <fuga@studiofuga.com>"
    url = "https://github.com/studiofuga/mSqliteCpp"
    description = "A modern SQLite C++ interface"
    topics = ("sqlite3", "c++")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": True}
    generators = "cmake_find_package"
    exports_sources = "*"

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.configure(source_folder=".", args=["-DENABLE_TEST=OFF"])
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def build_requirements(self):
        self.build_requires("boost/[1.69.0]@conan/stable")
        self.build_requires("sqlite3/[3.21.0]@bincrafters/stable")

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["msqlitecpp"]
