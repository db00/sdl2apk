git init
git add .
git commit 

git remote -v


ssh-keygen -t rsa -C "db0@qq.com"
#cp ~/.ssh/id_rsa.pub to github(Settings/SSH Keys)
ssh -T git@github.com


#push an existing repository
git remote add huaien pi@rpi:~/sdl2apk
git push -u huaien master
git remote -v
git pull huaien


git remote rm huaien



git config --global push.default simple
git config --global user.name "db0"
git config --global user.email db0@qq.com
git push --set-upstream origin master


