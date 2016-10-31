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
ssh -T git@github.com
