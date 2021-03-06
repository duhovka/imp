from __future__ import print_function
import IMP.test
import IMP.mmcif.metadata
import IMP.mmcif.data

class MockSystem(object):
    def __init__(self):
        self._external_files = IMP.mmcif.data._ExternalFiles()
        self.datasets = IMP.mmcif.data._Datasets(self._external_files)
        self._software = IMP.mmcif.data._AllSoftware()


class Tests(IMP.test.TestCase):
    def _parse_pdb(self, fname, chain):
        system = MockSystem()
        p = IMP.mmcif.metadata._PDBMetadataParser()
        p.parse_file(fname, chain, system)
        return p

    def test_official_pdb(self):
        """Test PDBMetadataParser when given an official PDB"""
        p = self._parse_pdb(self.get_input_file_name('official.pdb'), 'A')
        s, = p.sources
        self.assertEqual(s.db_code, '2HBJ')
        self.assertEqual(s.chain_id, 'A')
        self.assertEqual(p.dataset._data_type, 'Experimental model')
        self.assertEqual(p.dataset.location.db_name, 'PDB')
        self.assertEqual(p.dataset.location.access_code, '2HBJ')
        self.assertEqual(p.dataset.location.version, '14-JUN-06')
        self.assertEqual(p.dataset.location.details,
                         'STRUCTURE OF THE YEAST NUCLEAR EXOSOME COMPONENT, '
                         'RRP6P, REVEALS AN INTERPLAY BETWEEN THE ACTIVE '
                         'SITE AND THE HRDC DOMAIN')

    def test_derived_pdb(self):
        """Test PDBMetadataParser when given a file derived from a PDB"""
        pdbname = self.get_input_file_name('derived_pdb.pdb')
        p = self._parse_pdb(pdbname, 'A')
        s, = p.sources
        self.assertIs(s.db_code, None)
        self.assertEqual(s.chain_id, 'A')
        self.assertEqual(p.dataset._data_type, 'Experimental model')
        self.assertEqual(p.dataset.location.path, pdbname)
        self.assertEqual(p.dataset.location.repo, None)
        self.assertEqual(p.dataset.location.details,
                         'MED7C AND MED21 STRUCTURES FROM PDB ENTRY 1YKH, '
                         'ROTATED AND TRANSLATED TO ALIGN WITH THE '
                         'MED4-MED9 MODEL')
        parent, = p.dataset._parents
        self.assertEqual(parent._data_type, 'Experimental model')
        self.assertEqual(parent.location.db_name, 'PDB')
        self.assertEqual(parent.location.access_code, '1YKH')
        self.assertEqual(parent.location.version, None)
        self.assertEqual(parent.location.details, None)

    def test_derived_model(self):
        """Test PDBMetadataParser when given a file derived from a model"""
        pdbname = self.get_input_file_name('derived_model.pdb')
        p = self._parse_pdb(pdbname, 'A')
        s, = p.sources
        self.assertIs(s.db_code, None)
        self.assertEqual(s.chain_id, 'A')
        self.assertEqual(p.dataset._data_type, 'Comparative model')
        self.assertEqual(p.dataset.location.path, pdbname)
        self.assertEqual(p.dataset.location.repo, None)
        self.assertEqual(p.dataset.location.details,
                         'MED4 AND MED9 STRUCTURE TAKEN FROM LARIVIERE '
                         'ET AL, NUCLEIC ACIDS RESEARCH. 2013;41:9266-9273. '
                         'DOI: 10.1093/nar/gkt704. THE MED10 STRUCTURE ALSO '
                         'PROPOSED IN THAT WORK IS NOT USED IN THIS STUDY.')
        parent, = p.dataset._parents
        self.assertEqual(parent._data_type, 'Comparative model')
        self.assertEqual(parent.location.path, '.')
        self.assertEqual(parent.location.repo.doi, '10.1093/nar/gkt704')
        self.assertEqual(parent.location.details,
                         'Starting comparative model structure')

    def test_modeller_model_aln(self):
        """Test PDBMetadataParser when given a Modeller model with alignment"""
        pdbname = self.get_input_file_name('modeller_model.pdb')
        p = self.check_modeller_model(pdbname)
        self.assertEqual(p.alignment_file.path,
                         self.get_input_file_name('modeller_model.ali'))

    def test_modeller_model_no_aln(self):
        "Test PDBMetadataParser when given a Modeller model with no alignment"
        pdbname = self.get_input_file_name('modeller_model_no_aln.pdb')
        p = self.check_modeller_model(pdbname)

    def check_modeller_model(self, pdbname):
        p = self._parse_pdb(pdbname, 'A')
        s1, s2 = p.sources
        self.assertIs(s1.db_code, None)
        self.assertEqual(s1.chain_id, 'A')
        self.assertEqual(s1.tm_db_code, '3JRO')
        self.assertEqual(s1.tm_chain_id, 'C')
        self.assertIs(s2.db_code, None)
        self.assertEqual(s2.chain_id, 'A')
        self.assertEqual(s2.tm_db_code, '3F3F')
        self.assertEqual(s2.tm_chain_id, 'G')
        self.assertEqual(p.dataset._data_type, 'Comparative model')
        self.assertEqual(p.dataset.location.path, pdbname)
        self.assertEqual(p.dataset.location.repo, None)
        self.assertEqual(p.dataset.location.details,
                         'Starting model structure')
        p1, p2 = p.dataset._parents
        self.assertEqual(p1._data_type, 'Experimental model')
        self.assertEqual(p1.location.db_name, 'PDB')
        self.assertEqual(p1.location.access_code, '3JRO')
        self.assertEqual(p1.location.version, None)
        self.assertEqual(p1.location.details, None)
        self.assertEqual(p2.location.access_code, '3F3F')
        return p

    def test_modeller_local(self):
        "Test PDBMetadataParser when given a Modeller model with local template"
        pdbname = self.get_input_file_name('modeller_model_local.pdb')
        p = self._parse_pdb(pdbname, 'A')
        s, = p.sources
        self.assertIs(s.db_code, None)
        self.assertEqual(s.chain_id, 'A')
        self.assertIs(s.tm_db_code, None)
        self.assertEqual(s.tm_chain_id, 'C')
        parent, = p.dataset._parents
        self.assertEqual(parent._data_type, 'Experimental model')
        self.assertEqual(parent.location.details,
                         'Template for comparative modeling')
        self.assertEqual(parent.location.path,
                         self.get_input_file_name('15133C.pdb'))

    def test_phyre2_model(self):
        """Test PDBMetadataParser when given a Phyre2 model."""
        pdbname = self.get_input_file_name('phyre2_model.pdb')
        p = self._parse_pdb(pdbname, 'A')
        s, = p.sources
        self.assertIs(s.db_code, None)
        self.assertEqual(s.chain_id, 'A')
        self.assertEqual(s.tm_db_code, '4BZK')
        self.assertEqual(s.tm_chain_id, 'A')
        self.assertEqual(p.dataset._data_type, 'Comparative model')
        self.assertEqual(p.dataset.location.path, pdbname)
        self.assertIs(p.dataset.location.repo, None)
        self.assertEqual(p.dataset.location.details,
                         'Starting model structure')
        parent, = p.dataset._parents
        self.assertEqual(parent._data_type, 'Experimental model')
        self.assertEqual(parent.location.db_name, 'PDB')
        self.assertEqual(parent.location.access_code, '4BZK')
        self.assertIs(parent.location.version, None)
        self.assertIs(parent.location.details, None)



if __name__ == '__main__':
    IMP.test.main()
