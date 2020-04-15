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

project "pt"
    kind "SharedLib"
    files "src/pt/**"

project "app"
    kind "ConsoleApp"
    files "src/app/**"

    links "pt"
    includedirs "./src/pt/"

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
        os.execute("premake5 gmake2 && cd makefiles && make && (../build/bin/debug/app; echo $?)")
    end
}
