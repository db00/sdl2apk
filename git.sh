git init
git add .
git commit 

git remote -v
git remote add origin https://github.com/db00/sdl2apk.git
git push -u origin master


#push an existing repository
git remote add pi pi@rpi:~/sdl2apk
git push -u pi master
git remote -v
git pull pi


git remote rm huaien


git config --global push.default simple
git config --global user.name "db0"
git config --global user.email db0@qq.com
git push --set-upstream origin master


ssh-keygen -t rsa -C "db0@qq.com"
#cp ~/.ssh/id_rsa.pub to github(Settings/SSH Keys)
ssh -T git@github.com
