source $HOME/.bashrc
#/usr/local/bin/screen -d -RR -e ^Ww

export P4PORT=perforce.ironport.com:1666
export P4CONFIG=P4ENV

if [ -f ~/.aprc ]
then
    . ~/.aprc
fi

skill () {
      kill -9 `ps ax | grep $1 | grep -v grep | awk '{print $1}'`
  }

