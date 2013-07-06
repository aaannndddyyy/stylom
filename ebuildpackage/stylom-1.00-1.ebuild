# $Header: $

EAPI=4

DESCRIPTION="Tool to facilitate the stylometric analysis of texts. It could be used for academic disambiguation of disputed authorship, and to help identify plagiarists, astroturfers, sockpuppets and guerilla marketers. Another possible use case is as an assistance to the anonymisation of writing style.  Every author has their own unique writing style, and if enough writing examples are available then it's possible to construct a quantitative model of their style which can be compared against others."
HOMEPAGE="https://github.com/fuzzgun/stylom"
SRC_URI="${PN}/${P}.tar.gz"
LICENSE="GPL3"
SLOT="0"
KEYWORDS="x86"
RDEPEND="dev-libs/popt"
DEPEND="${RDEPEND}"

src_configure() {
    econf --with-popt
}

src_install() {
    emake DESTDIR="${D}" install
    # Install README and (Debian) changelog
    dodoc README.md debian/changelog
}
