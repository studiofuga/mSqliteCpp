#!/usr/bin/env python
# -*- coding: utf-8 -*-

from conan import ConanFile, tools
from conan.tools.files import get,copy
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
import os

class SparseppConan(ConanFile):
    name = "msqlitecpp"
    version = "1.99.6.3"
    license = "BSD3"
    author = "Federico Fuga <fuga@studiofuga.com>"
    url = "https://github.com/studiofuga/mSqliteCpp"
    description = "A modern SQLite C++ interface"
    topics = ("sqlite3", "c++")

    exports_sources = "*"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    options = {"shared": [True, False],
               "fPIC": [True, False]}

    default_options = {"shared": True,
                       "fPIC": True}

    test_requires = "gtest/1.16.0"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def build_requirements(self):
        self.build_requires("boost/[>=1.71.0]")
        self.build_requires("sqlite3/[>=3.35.5]")

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["msqlitecpp"]
        self.cpp_info.set_property("cmake_file_name", "msqlitecpp")
        self.cpp_info.set_property("cmake_target_name", "msqlitecpp::msqlitecpp")

        self.cpp_info.builddirs.append(os.path.join("lib", "cmake", self.name))

        self.cpp_info.names["cmake_find_package"] = "msqlitecpp"
        self.cpp_info.names["cmake_find_package_multi"] = "msqlitecpp"
