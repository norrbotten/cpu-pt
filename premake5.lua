workspace "cpu-pt"
    language "C++"
    cppdialect "C++17"

    location "makefiles"
    
    configurations { "debug", "release" }

    filter { "configurations:debug" }
        targetdir "build/bin/debug"
        objdir "build/obj/debug"

        symbols "on"
    
    filter { "configurations:release" }
        targetdir "build/bin/release"
        objdir "build/obj/release"

        optimize "on"
    
    filter { }

    includedirs { "src/", "ext/eigen/", "ext/lodepng/" }

project "lodepng"
    kind "SharedLib"
    files "ext/lodepng/lodepng.cpp"

project "pt"
    kind "SharedLib"
    files "src/pt/**"

project "app"
    kind "ConsoleApp"
    files "src/app/**"

    links { "pt", "lodepng" }

newaction {
    trigger = "clean",
    description = "clean the build",
    execute = function()
        os.rmdir("./build")
        os.rmdir("./makefiles")
    end
}

newaction {
    trigger = "run",
    description = "compile a debug build and run",
    execute = function()
        os.execute("premake5 gmake2 && cd makefiles && make config=release && (cd .. && echo ---; ./build/bin/release/app; echo App returned: $?)")
    end
}
