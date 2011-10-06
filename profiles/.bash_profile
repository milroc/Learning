source $HOME/.bashrc
#/usr/local/bin/screen -d -RR -e ^Ww


if [ -f ~/.aprc ]
then
    . ~/.aprc
fi

skill () {
      kill -9 `ps ax | grep $1 | grep -v grep | awk '{print $1}'`
  }

