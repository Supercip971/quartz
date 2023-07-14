from cutekit import args, builder, cmds, shell, utils, const 
print(f"dir: {const.PROJECT_CK_DIR}");
shaderFiles = shell.find('src/', ['*.vs', '*.fs'])

shaderOutputDir = const.PROJECT_CK_DIR + '/build/'
for shaderFile in  shaderFiles:
    shaderType = ""
    if( shaderFile.endswith('.vs') ):
        shaderType = 'vertex'
    else:
        shaderType = 'fragment'
    BuiltShaderOutFile = shaderFile.replace('.vs', '.o').replace('.fs', '.o')

    shell.mkdir(shaderOutputDir)

    BuiltShaderOutFile = BuiltShaderOutFile.replace('src/', shaderOutputDir)
    shell.mkdir(shaderOutputDir + '/shaders/')


    if utils.isNewer(shaderFile, BuiltShaderOutFile):
        print(f"compiling {shaderFile} to {BuiltShaderOutFile}")
        stage = '-fshader-stage=vert' if shaderType == 'vertex' else '-fshader-stage=frag'
        shell.exec('glslc', "--target-env=vulkan1.3", "--target-spv=spv1.5", "-std=460core", "-Isrc/shaders", stage , shaderFile, "-o", BuiltShaderOutFile)


def createBundle(_args: args.Args, component: str):
    executable = builder.build(component, 'host-x86_64')
    shell.mkdir(const.PROJECT_CK_DIR + '/build/bundles/' + component)
    shell.cp(executable.outfile(), const.PROJECT_CK_DIR + '/build/bundles/' + component + '/' + component)
  #  shell.cpTree(const.PROJECT_CK_DIR + '/assets', const.PROJECT_CK_DIR + '/build/bundles/' + component + '/assets')
    shell.cpTree(shaderOutputDir + '/shaders/', const.PROJECT_CK_DIR + '/build/bundles/' + component + '/shaders')
    print(f"copying to: {const.PROJECT_CK_DIR + '/build/bundles/' + component + '/shaders'}")
    print(f"copying to: {const.PROJECT_CK_DIR + '/build/bundles/' + 'shaders'}")

    shell.cpTree(shaderOutputDir + '/shaders/', const.PROJECT_CK_DIR + '/build/bundles/' + 'shaders')
    
    shell.exec(const.PROJECT_CK_DIR + '/build/bundles/' + component + '/' + component)



def run(_args: args.Args):
    createBundle(_args, 'hello-world')




cmds.append(cmds.Cmd('s', 'start', 'Run the application', run))
