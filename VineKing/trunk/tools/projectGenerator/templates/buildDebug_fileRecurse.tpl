{if is_array($dirWalk)}
{assign var="dirPrefix" value="<dir $dirName>"}
{$dirPrefix|indent:$depth:"\t"}
{foreach from=$dirWalk item=dir key=key}
{include file="buildDebug_fileRecurse.tpl" dirWalk=$dir dirName=$key depth=$depth+1}
{/foreach}
{assign var="dirSuffix" value="</dir>"}
{$dirSuffix|indent:$depth:"\t"}
{else}
{$dirWalk|indent:$depth:"\t"}
{/if}