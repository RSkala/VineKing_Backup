
The files:

{assign var="dirWalk" value=$fileArray}
{include file="buildDebug_fileRecurse.tpl" dirWalk=$dirWalk depth=1}
