%VULKAN_SDK%\bin\glslangvalidator -V default.vert -o default.vert.spv
%VULKAN_SDK%\bin\glslangvalidator -V default.frag -o default.frag.spv


@rem Test shader

%VULKAN_SDK%\bin\glslangvalidator -V .\test\vktriangle.vert -o .\test\vktriangle.vert.spv
%VULKAN_SDK%\bin\glslangvalidator -V .\test\vktriangle.frag -o .\test\vktriangle.frag.spv

@rem %VULKAN_SDK%\bin\glslangvalidator -V default.comp -o default.comp.spv
