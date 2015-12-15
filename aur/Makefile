all:
	@echo "Please 'make local' or 'make git'"

local:
	ln -s PKGBUILD.local PKGBUILD
	makepkg
	rm -rf src pkg PKGBUILD

remote:
	ln -s PKGBUILD.remote PKGBUILD
	makepkg
	rm -rf src pkg PKGBUILD

git:
	ln -s PKGBUILD.git PKGBUILD
	makepkg
	rm -rf src pkg PKGBUILD

clean:
	rm -rf src pkg PKGBUILD *.xz
