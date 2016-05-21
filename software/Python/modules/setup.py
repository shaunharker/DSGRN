
import setuptools
setuptools.setup(
    name='DSGRN',
    version = "0.1",
    description = "Python bindings for DSGRN",
    author = "Shaun Harker",
    author_email = "sharker@math.rutgers.edu",
    license="MIT LICENSE",
    packages=setuptools.find_packages(),
    zip_safe= False,
    package_data={'DSGRN': ['libpyDSGRN.so']},
)
