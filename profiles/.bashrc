alias lss='ls -lhartGA'
export CLICOLOR=1
export LSCOLORS=DxGxcxdxCxegedabagacad
export P4PORT='perforce.ironport.com:1666'
export P4CONFIG='P4ENV'
export P4USER='mmccrock'
export IPROOT=$HOME/work/wsa-main
export IPPROD=coeus

export DEV_CONFIG_PORT_PREFIX=29

if [ -f ~/.aprc ]
then
   .  ~/.aprc
fi


source $HOME/.aprc
