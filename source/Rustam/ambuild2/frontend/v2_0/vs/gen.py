# vim: set ts=8 sts=4 sw=4 tw=99 et:
#
# This file is part of AMBuild.
#
# AMBuild is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# AMBuild is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with AMBuild. If not, see <http://www.gnu.org/licenses/>.

from ambuild2.frontend.vs import gen as vs_gen
from ambuild2.frontend.v2_0.vs import cxx

class Generator(vs_gen.Generator):
    def __init__(self, cm):
        super(Generator, self).__init__(cm)

    def detectCompilers(self):
        if not self.compiler:
            self.base_compiler = cxx.Compiler(cxx.Compiler.GetVersionFromVS(self.vs_version))
            self.compiler = self.base_compiler.clone()
        return self.compiler
