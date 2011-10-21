##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=Lagom
ConfigurationName      :=Debug
IntermediateDirectory  :=../../out/x64/Debug
OutDir                 := $(IntermediateDirectory)
WorkspacePath          := "/home/jaschmid/Dropbox/School/TDDD23/Lagom/projects/codelite"
ProjectPath            := "/home/jaschmid/Dropbox/School/TDDD23/Lagom/projects/codelite"
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Jan Schmid
Date                   :=09/19/2011
CodeLitePath           :="/home/jaschmid/.codelite"
LinkerName             :=g++
ArchiveTool            :=ar rcus
SharedObjectLinkerName :=g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.o.i
DebugSwitch            :=-gstab
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
CompilerName           :=g++
C_CompilerName         :=gcc
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E 
ObjectsFileList        :="/home/jaschmid/Dropbox/School/TDDD23/Lagom/projects/codelite/Lagom.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
CmpOptions             := -g -std=c++0x $(Preprocessors)
C_CmpOptions           := -g $(Preprocessors)
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)/usr/include/bullet $(IncludeSwitch)/usr/include/CEGUI 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)OgreMain $(LibrarySwitch)OIS 
LibPath                := $(LibraryPathSwitch). 


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects=$(IntermediateDirectory)/src_AppStateManager$(ObjectSuffix) $(IntermediateDirectory)/src_DemoApp$(ObjectSuffix) $(IntermediateDirectory)/src_GameState$(ObjectSuffix) $(IntermediateDirectory)/src_Lagom$(ObjectSuffix) $(IntermediateDirectory)/src_main$(ObjectSuffix) $(IntermediateDirectory)/src_MenuState$(ObjectSuffix) $(IntermediateDirectory)/src_PauseState$(ObjectSuffix) $(IntermediateDirectory)/src_lagom_actor$(ObjectSuffix) $(IntermediateDirectory)/src_lagom_actor_factory$(ObjectSuffix) $(IntermediateDirectory)/src_lagom_unit$(ObjectSuffix) \
	$(IntermediateDirectory)/src_lagom_player_base$(ObjectSuffix) $(IntermediateDirectory)/src_lagom_unit_glowsphere$(ObjectSuffix) $(IntermediateDirectory)/src_MenuButton$(ObjectSuffix) $(IntermediateDirectory)/src_MenuEntry$(ObjectSuffix) $(IntermediateDirectory)/src_MenuManager$(ObjectSuffix) $(IntermediateDirectory)/src_MenuSlider$(ObjectSuffix) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects) > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

$(IntermediateDirectory)/.d:
	@test -d ../../out/x64/Debug || $(MakeDirCommand) ../../out/x64/Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/src_AppStateManager$(ObjectSuffix): ../../src/AppStateManager.cpp $(IntermediateDirectory)/src_AppStateManager$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/AppStateManager.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/src_AppStateManager$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_AppStateManager$(DependSuffix): ../../src/AppStateManager.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_AppStateManager$(ObjectSuffix) -MF$(IntermediateDirectory)/src_AppStateManager$(DependSuffix) -MM "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/AppStateManager.cpp"

$(IntermediateDirectory)/src_AppStateManager$(PreprocessSuffix): ../../src/AppStateManager.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_AppStateManager$(PreprocessSuffix) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/AppStateManager.cpp"

$(IntermediateDirectory)/src_DemoApp$(ObjectSuffix): ../../src/DemoApp.cpp $(IntermediateDirectory)/src_DemoApp$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/DemoApp.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/src_DemoApp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_DemoApp$(DependSuffix): ../../src/DemoApp.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_DemoApp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_DemoApp$(DependSuffix) -MM "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/DemoApp.cpp"

$(IntermediateDirectory)/src_DemoApp$(PreprocessSuffix): ../../src/DemoApp.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_DemoApp$(PreprocessSuffix) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/DemoApp.cpp"

$(IntermediateDirectory)/src_GameState$(ObjectSuffix): ../../src/GameState.cpp $(IntermediateDirectory)/src_GameState$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/GameState.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/src_GameState$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_GameState$(DependSuffix): ../../src/GameState.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_GameState$(ObjectSuffix) -MF$(IntermediateDirectory)/src_GameState$(DependSuffix) -MM "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/GameState.cpp"

$(IntermediateDirectory)/src_GameState$(PreprocessSuffix): ../../src/GameState.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_GameState$(PreprocessSuffix) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/GameState.cpp"

$(IntermediateDirectory)/src_Lagom$(ObjectSuffix): ../../src/Lagom.cpp $(IntermediateDirectory)/src_Lagom$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/Lagom.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/src_Lagom$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Lagom$(DependSuffix): ../../src/Lagom.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Lagom$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Lagom$(DependSuffix) -MM "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/Lagom.cpp"

$(IntermediateDirectory)/src_Lagom$(PreprocessSuffix): ../../src/Lagom.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Lagom$(PreprocessSuffix) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/Lagom.cpp"

$(IntermediateDirectory)/src_main$(ObjectSuffix): ../../src/main.cpp $(IntermediateDirectory)/src_main$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/main.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/src_main$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_main$(DependSuffix): ../../src/main.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_main$(ObjectSuffix) -MF$(IntermediateDirectory)/src_main$(DependSuffix) -MM "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/main.cpp"

$(IntermediateDirectory)/src_main$(PreprocessSuffix): ../../src/main.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_main$(PreprocessSuffix) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/main.cpp"

$(IntermediateDirectory)/src_MenuState$(ObjectSuffix): ../../src/MenuState.cpp $(IntermediateDirectory)/src_MenuState$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/MenuState.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/src_MenuState$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_MenuState$(DependSuffix): ../../src/MenuState.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_MenuState$(ObjectSuffix) -MF$(IntermediateDirectory)/src_MenuState$(DependSuffix) -MM "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/MenuState.cpp"

$(IntermediateDirectory)/src_MenuState$(PreprocessSuffix): ../../src/MenuState.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_MenuState$(PreprocessSuffix) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/MenuState.cpp"

$(IntermediateDirectory)/src_PauseState$(ObjectSuffix): ../../src/PauseState.cpp $(IntermediateDirectory)/src_PauseState$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/PauseState.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/src_PauseState$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_PauseState$(DependSuffix): ../../src/PauseState.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_PauseState$(ObjectSuffix) -MF$(IntermediateDirectory)/src_PauseState$(DependSuffix) -MM "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/PauseState.cpp"

$(IntermediateDirectory)/src_PauseState$(PreprocessSuffix): ../../src/PauseState.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_PauseState$(PreprocessSuffix) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/PauseState.cpp"

$(IntermediateDirectory)/src_lagom_actor$(ObjectSuffix): ../../src/lagom_actor.cpp $(IntermediateDirectory)/src_lagom_actor$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/lagom_actor.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/src_lagom_actor$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_lagom_actor$(DependSuffix): ../../src/lagom_actor.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_lagom_actor$(ObjectSuffix) -MF$(IntermediateDirectory)/src_lagom_actor$(DependSuffix) -MM "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/lagom_actor.cpp"

$(IntermediateDirectory)/src_lagom_actor$(PreprocessSuffix): ../../src/lagom_actor.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_lagom_actor$(PreprocessSuffix) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/lagom_actor.cpp"

$(IntermediateDirectory)/src_lagom_actor_factory$(ObjectSuffix): ../../src/lagom_actor_factory.cpp $(IntermediateDirectory)/src_lagom_actor_factory$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/lagom_actor_factory.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/src_lagom_actor_factory$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_lagom_actor_factory$(DependSuffix): ../../src/lagom_actor_factory.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_lagom_actor_factory$(ObjectSuffix) -MF$(IntermediateDirectory)/src_lagom_actor_factory$(DependSuffix) -MM "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/lagom_actor_factory.cpp"

$(IntermediateDirectory)/src_lagom_actor_factory$(PreprocessSuffix): ../../src/lagom_actor_factory.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_lagom_actor_factory$(PreprocessSuffix) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/lagom_actor_factory.cpp"

$(IntermediateDirectory)/src_lagom_unit$(ObjectSuffix): ../../src/lagom_unit.cpp $(IntermediateDirectory)/src_lagom_unit$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/lagom_unit.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/src_lagom_unit$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_lagom_unit$(DependSuffix): ../../src/lagom_unit.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_lagom_unit$(ObjectSuffix) -MF$(IntermediateDirectory)/src_lagom_unit$(DependSuffix) -MM "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/lagom_unit.cpp"

$(IntermediateDirectory)/src_lagom_unit$(PreprocessSuffix): ../../src/lagom_unit.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_lagom_unit$(PreprocessSuffix) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/lagom_unit.cpp"

$(IntermediateDirectory)/src_lagom_player_base$(ObjectSuffix): ../../src/lagom_player_base.cpp $(IntermediateDirectory)/src_lagom_player_base$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/lagom_player_base.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/src_lagom_player_base$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_lagom_player_base$(DependSuffix): ../../src/lagom_player_base.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_lagom_player_base$(ObjectSuffix) -MF$(IntermediateDirectory)/src_lagom_player_base$(DependSuffix) -MM "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/lagom_player_base.cpp"

$(IntermediateDirectory)/src_lagom_player_base$(PreprocessSuffix): ../../src/lagom_player_base.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_lagom_player_base$(PreprocessSuffix) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/lagom_player_base.cpp"

$(IntermediateDirectory)/src_lagom_unit_glowsphere$(ObjectSuffix): ../../src/lagom_unit_glowsphere.cpp $(IntermediateDirectory)/src_lagom_unit_glowsphere$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/lagom_unit_glowsphere.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/src_lagom_unit_glowsphere$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_lagom_unit_glowsphere$(DependSuffix): ../../src/lagom_unit_glowsphere.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_lagom_unit_glowsphere$(ObjectSuffix) -MF$(IntermediateDirectory)/src_lagom_unit_glowsphere$(DependSuffix) -MM "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/lagom_unit_glowsphere.cpp"

$(IntermediateDirectory)/src_lagom_unit_glowsphere$(PreprocessSuffix): ../../src/lagom_unit_glowsphere.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_lagom_unit_glowsphere$(PreprocessSuffix) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/lagom_unit_glowsphere.cpp"

$(IntermediateDirectory)/src_MenuButton$(ObjectSuffix): ../../src/MenuButton.cpp $(IntermediateDirectory)/src_MenuButton$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/MenuButton.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/src_MenuButton$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_MenuButton$(DependSuffix): ../../src/MenuButton.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_MenuButton$(ObjectSuffix) -MF$(IntermediateDirectory)/src_MenuButton$(DependSuffix) -MM "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/MenuButton.cpp"

$(IntermediateDirectory)/src_MenuButton$(PreprocessSuffix): ../../src/MenuButton.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_MenuButton$(PreprocessSuffix) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/MenuButton.cpp"

$(IntermediateDirectory)/src_MenuEntry$(ObjectSuffix): ../../src/MenuEntry.cpp $(IntermediateDirectory)/src_MenuEntry$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/MenuEntry.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/src_MenuEntry$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_MenuEntry$(DependSuffix): ../../src/MenuEntry.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_MenuEntry$(ObjectSuffix) -MF$(IntermediateDirectory)/src_MenuEntry$(DependSuffix) -MM "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/MenuEntry.cpp"

$(IntermediateDirectory)/src_MenuEntry$(PreprocessSuffix): ../../src/MenuEntry.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_MenuEntry$(PreprocessSuffix) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/MenuEntry.cpp"

$(IntermediateDirectory)/src_MenuManager$(ObjectSuffix): ../../src/MenuManager.cpp $(IntermediateDirectory)/src_MenuManager$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/MenuManager.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/src_MenuManager$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_MenuManager$(DependSuffix): ../../src/MenuManager.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_MenuManager$(ObjectSuffix) -MF$(IntermediateDirectory)/src_MenuManager$(DependSuffix) -MM "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/MenuManager.cpp"

$(IntermediateDirectory)/src_MenuManager$(PreprocessSuffix): ../../src/MenuManager.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_MenuManager$(PreprocessSuffix) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/MenuManager.cpp"

$(IntermediateDirectory)/src_MenuSlider$(ObjectSuffix): ../../src/MenuSlider.cpp $(IntermediateDirectory)/src_MenuSlider$(DependSuffix)
	$(CompilerName) $(IncludePCH) $(SourceSwitch) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/MenuSlider.cpp" $(CmpOptions) $(ObjectSwitch)$(IntermediateDirectory)/src_MenuSlider$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_MenuSlider$(DependSuffix): ../../src/MenuSlider.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_MenuSlider$(ObjectSuffix) -MF$(IntermediateDirectory)/src_MenuSlider$(DependSuffix) -MM "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/MenuSlider.cpp"

$(IntermediateDirectory)/src_MenuSlider$(PreprocessSuffix): ../../src/MenuSlider.cpp
	@$(CompilerName) $(CmpOptions) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_MenuSlider$(PreprocessSuffix) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/src/MenuSlider.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) $(IntermediateDirectory)/src_AppStateManager$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_AppStateManager$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_AppStateManager$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_DemoApp$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_DemoApp$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_DemoApp$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_GameState$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_GameState$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_GameState$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_Lagom$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_Lagom$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_Lagom$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_main$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_main$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_main$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_MenuState$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_MenuState$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_MenuState$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_PauseState$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_PauseState$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_PauseState$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_lagom_actor$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_lagom_actor$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_lagom_actor$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_lagom_actor_factory$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_lagom_actor_factory$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_lagom_actor_factory$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_lagom_unit$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_lagom_unit$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_lagom_unit$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_lagom_player_base$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_lagom_player_base$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_lagom_player_base$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_lagom_unit_glowsphere$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_lagom_unit_glowsphere$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_lagom_unit_glowsphere$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_MenuButton$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_MenuButton$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_MenuButton$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_MenuEntry$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_MenuEntry$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_MenuEntry$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_MenuManager$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_MenuManager$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_MenuManager$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_MenuSlider$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_MenuSlider$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_MenuSlider$(PreprocessSuffix)
	$(RM) $(OutputFile)
	$(RM) "/home/jaschmid/Dropbox/School/TDDD23/Lagom/projects/codelite/.build-debug/Lagom"


