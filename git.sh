git init
git add .
git commit 

git remote add test pi@rpi:~/sdl2apk
git push -u test master

git remote -v

git pull (=git fetch test && git merge test/master)
git push test/master

git remote rm test

ssh-keygen -t rsa -C "db0@qq.com"
#cp ~/.ssh/id_rsa.pub to github(Settings/SSH Keys)
ssh -T git@github.com


git config --global push.default simple
git config --global user.name "db0"
git config --global user.email db0@qq.com
git push --set-upstream origin master


#push an existing repository
git remote add huaien pi@rpi:~/sdl2apk
git push -u huaien master
git remote -v
git pull huaien
