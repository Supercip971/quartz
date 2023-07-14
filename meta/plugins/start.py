from cutekit import args, builder, cmds, shell, utils, const 
print(f"dir: {const.PROJECT_CK_DIR}");
shaderFiles = shell.find('src/', ['*.vs', '*.fs'])

for shaderFile in  shaderFiles:
    shaderType = ""
    if( shaderFile.endswith('.vs') ):
        shaderType = 'vertex'
    else:
        shaderType = 'fragment'
    BuiltShaderOutFile = shaderFile.replace('.vs', '.o').replace('.fs', '.o')

    shaderOutputDir = const.PROJECT_CK_DIR + '/build/'
    shell.mkdir(shaderOutputDir)

    BuiltShaderOutFile = BuiltShaderOutFile.replace('src/', shaderOutputDir)
    shell.mkdir(shaderOutputDir + '/shaders/')


    if utils.isNewer(shaderFile, BuiltShaderOutFile):
        print(f"compiling {shaderFile} to {BuiltShaderOutFile}")
        stage = '-fshader-stage=vert' if shaderType == 'vertex' else '-fshader-stage=frag'
        shell.exec('glslc', "--target-env=vulkan1.3", "--target-spv=spv1.5", "-std=460core", "-Isrc/shaders", stage , shaderFile, "-o", BuiltShaderOutFile)

def run(_args: args.Args):
    executable = builder.build('hello-world', 'host-x86_64')
    shell.exec(executable.outfile())



cmds.append(cmds.Cmd('s', 'start', 'Run the application', run))
