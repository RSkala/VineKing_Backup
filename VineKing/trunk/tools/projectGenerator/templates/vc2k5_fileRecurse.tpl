{*                                   *}
{*      Is this a dir or an item?    *}
{*                                   *}
{if is_array($dirWalk)}
{if $depth > 2}
{*                                   *}
{*      Generate dir prefix          *}
{*                                   *}
{capture assign="dirPrefix"}
<Filter
	Name="{$dirName}"
	Filter="">{/capture}
{$dirPrefix|indent:$depth:"\t"}
{/if}
{*                                   *}
{*      Iterate over children        *}
{*                                   *}
{foreach from=$dirWalk item=dir key=key}
{include file="vc2k5_fileRecurse.tpl" dirWalk=$dir dirName=$key dirPath="$dirPath$dirName/" depth=$depth+1}
{/foreach}
{if $depth > 2}
{*                                   *}
{*      Generate dir suffix          *}
{*                                   *}
{capture assign="dirSuffix"}
</Filter>{/capture}
{$dirSuffix|indent:$depth:"\t"}
{/if}
{else}
{*                                   *}
{*      Output an item               *}
{*                                   *}
{capture assign="itemOut"}
<File
	RelativePath="{$dirPath|replace:'//':'/'|replace:'/':'\\'}{$dirWalk}">
{if substr($dirWalk, -4, 4) == ".asm"}
	<FileConfiguration
		Name="Debug|Win32">
		<Tool
			Name="VCCustomBuildTool"
			CommandLine="&quot;$(SolutionDir)..\..\Bin\nasm\nasmw.exe&quot; -f win32 &quot;$(InputPath)&quot; -o &quot;$(IntDir)/$(InputName).obj&quot;"
			Outputs="$(IntDir)/$(InputName).obj"/>
	</FileConfiguration>
	<FileConfiguration
		Name="Release|Win32">
		<Tool
			Name="VCCustomBuildTool"
			CommandLine="&quot;$(SolutionDir)..\..\Bin\nasm\nasmw.exe&quot; -f win32 &quot;$(InputPath)&quot; -o &quot;$(IntDir)/$(InputName).obj&quot;"
			Outputs="$(IntDir)/$(InputName).obj"/>
	</FileConfiguration>
{/if}
</File>{/capture}
{$itemOut|indent:$depth:"\t"}
{/if}