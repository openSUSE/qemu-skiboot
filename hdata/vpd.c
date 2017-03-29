/* Copyright 2013-2014 IBM Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <skiboot.h>
#include <vpd.h>
#include <string.h>
#include "spira.h"
#include "hdata.h"
#include <device.h>
#include "hdata.h"
#include <inttypes.h>

struct card_info {
	const char *ccin; 	/* Customer card identification number */
	const char *description;
};

static const struct card_info card_table[] = {
	{"2B06", "System planar 2S4U"},
	{"2B07", "System planar 1S4U"},
	{"2B2E", "System planar 2S2U"},
	{"2B2F", "System planar 1S2U"},
	{"2CD4", "System planar 2S4U"},
	{"2CD5", "System planar 1S4U"},
	{"2CD6", "System planar 2S2U"},
	{"2CD7", "System planar 1S2U"},
	{"2CD7", "System planar 1S2U"},
	{"2B09", "Base JBOD, RAID and Backplane HD"},
	{"57D7", "Split JBOD, RAID Card"},
	{"2B0B", "Native I/O Card"},

	/* Anchor cards */
	{"52FE", "System Anchor Card - IBM Power 824"},
	{"52F2", "System Anchor Card - IBM Power 814"},
	{"52F5", "System Anchor Card - IBM Power 822"},
	{"561A", "System Anchor Card - IBM Power 824L"},
	{"524D", "System Anchor Card - IBM Power 822L"},
	{"560F", "System Anchor Card - IBM Power 812L"},
	{"561C", "System Anchor Card - DS8870"},

	/* Memory DIMMs */
	{"31E0", "16GB CDIMM"},
	{"31E8", "16GB CDIMM"},
	{"31E1", "32GB CDIMM"},
	{"31E9", "32GB CDIMM"},
	{"31E2", "64GB CDIMM"},
	{"31EA", "64GB CDIMM"},

	/* Power supplies */
	{"2B1D", "Power Supply 900W AC"},
	{"2B1E", "Power Supply 1400W AC"},
	{"2B75", "Power Supply 1400W HVDC"},

	/* Fans */
	{"2B1F", "Fan 4U (A1, A2, A3, A4)"},
	{"2B29", "Fan 2U (A1, A2, A3, A4, A5, A6)"},

	/* Other cards */
};

struct vpd_key_map {
	const char *keyword;		/* 2 char keyword  */
	const char *description;
};

static const struct vpd_key_map vpd_key_table[] = {
	{"AA", "ac-power-supply"},
	{"AM", "air-mover"},
	{"AV", "anchor-card"},

	{"BA", "bus-adapter-card"},
	{"BC", "battery-charger"},
	{"BD", "bus-daughter-card"},
	{"BE", "bus-expansion-card"},
	{"BP", "backplane"},
	{"BR", "backplane-riser"},
	{"BX", "backplane-extender"},

	{"CA", "calgary-bridge"},
	{"CB", "infiniband-connector"},
	{"CC", "clock-card"},
	{"CD", "card-connector"},
	{"CE", "ethernet-connector"},
	{"CL", "calgary-phb"},
	{"CI", "capacity-card"},
	{"CO", "sma-connector"},
	{"CP", "processor-capacity-card"},
	{"CR", "rio-connector"},
	{"CS", "serial-connector"},
	{"CU", "usb-connector"},

	{"DB", "dasd-backplane"},
	{"DC", "drawer-card"},
	{"DE", "drawer-extension"},
	{"DI", "drawer-interposer"},
	{"DL", "p7ih-dlink-connector"},
	{"DT", "legacy-pci-card"},
	{"DV", "media-drawer-led"},

	{"EI", "enclosure-led"},
	{"EF", "enclosure-fault-led"},
	{"ES", "embedded-sas"},
	{"ET", "ethernet-riser"},
	{"EV", "enclosure"},

	{"FM", "frame"},

	{"HB", "host-rio-pci-card"},
	{"HD", "high-speed-card"},
	{"HM", "hmc-connector"},

	{"IB", "io-backplane"},
	{"IC", "io-card"},
	{"ID", "ide-connector"},
	{"II", "io-drawer-led"},
	{"IP", "interplane-card"},
	{"IS", "smp-vbus-cable"},
	{"IT", "enclosure-cable"},
	{"IV", "io-enclosure"},

	{"KV", "keyboard-led"},

	{"L2", "l2-cache-module"},
	{"L3", "l3-cache-module"},
	{"LC", "squadrons-light-connector"},
	{"LR", "p7ih-connector"},
	{"LO", "system-locate-led"},
	{"LT", "squadrons-light-strip"},

	{"MB", "media-backplane"},
	{"ME", "map-extension"},
	{"MM", "mip-meter"},
	{"MS", "ms-dimm"},

	{"NB", "nvram-battery"},
	{"NC", "sp-node-controller"},
	{"ND", "numa-dimm"},

	{"OD", "cuod-card"},
	{"OP", "op-panel"},
	{"OS", "oscillator"},

	{"P2", "ioc"},
	{"P5", "ioc-bridge"},
	{"PB", "io-drawer-backplane"},
	{"PC", "power-capacitor"},
	{"PD", "processor-card"},
	{"PF", "processor"},
	{"PI", "ioc-phb"},
	{"PO", "spcn"},
	{"PN", "spcn-connector"},
	{"PR", "pci-riser-card"},
	{"PS", "power-supply"},
	{"PT", "pass-through-card"},
	{"PX", "psc-sync-card"},
	{"PW", "power-connector"},

	{"RG", "regulator"},
	{"RI", "riser"},
	{"RK", "rack-indicator"},
	{"RW", "riscwatch-connector"},

	{"SA", "sys-attn-led"},
	{"SB", "backup-sysvpd"},
	{"SC", "scsi-connector"},
	{"SD", "sas-connector"},
	{"SI", "scsi-ide-converter"},
	{"SL", "phb-slot"},
	{"SP", "service-processor"},
	{"SR", "service-card"},
	{"SS", "soft-switch"},
	{"SV", "system-vpd"},
	{"SY", "legacy-sysvpd"},

	{"TD", "tod-clock"},
	{"TI", "torrent-pcie-phb"},
	{"TL", "torrent-riser"},
	{"TM", "thermal-sensor"},
	{"TP", "tpmd-adapter"},
	{"TR", "torrent-bridge"},

	{"VV", "root-node-vpd"},

	{"WD", "water_device"},
};

static const char *vpd_map_name(const char *vpd_name)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(vpd_key_table); i++)
		if (!strcmp(vpd_key_table[i].keyword, vpd_name))
			return vpd_key_table[i].description;

	prlog(PR_WARNING, "VPD: Could not map FRU ID %s to a known name\n",
	      vpd_name);

	return "Unknown";
}

static struct dt_node *dt_create_vpd_node(struct dt_node *parent,
					  const struct slca_entry *entry);

static const struct card_info *card_info_lookup(char *ccin)
{
	int i;
	for(i = 0; i < ARRAY_SIZE(card_table); i++)
		if (!strcmp(card_table[i].ccin, ccin))
			return &card_table[i];
	return NULL;
}

static void vpd_vini_parse(struct dt_node *node,
			   const void *fruvpd, unsigned int fruvpd_sz)
{
	const void *kw;
	char *str;
	uint8_t kwsz;
	const struct card_info *cinfo;

	/* FRU Stocking Part Number */
	kw = vpd_find(fruvpd, fruvpd_sz, "VINI", "FN", &kwsz);
	if (kw) {
		str = zalloc(kwsz + 1);
		if (!str)
			goto no_memory;
		memcpy(str, kw, kwsz);
		dt_add_property_string(node, "fru-number", str);
		free(str);
	}

	/* Serial Number */
	kw = vpd_find(fruvpd, fruvpd_sz, "VINI", "SN", &kwsz);
	if (kw) {
		str = zalloc(kwsz + 1);
		if (!str)
			goto no_memory;
		memcpy(str, kw, kwsz);
		dt_add_property_string(node, "serial-number", str);
		free(str);
	}

	/* Part Number */
	kw = vpd_find(fruvpd, fruvpd_sz, "VINI", "PN", &kwsz);
	if (kw) {
		str = zalloc(kwsz + 1);
		if (!str)
			goto no_memory;
		memcpy(str, kw, kwsz);
		dt_add_property_string(node, "part-number", str);
		free(str);
	}

	/* Customer Card Identification Number (CCIN) */
	kw = vpd_find(fruvpd, fruvpd_sz, "VINI", "CC", &kwsz);
	if (kw) {
		str = zalloc(kwsz + 1);
		if (!str)
			goto no_memory;
		memcpy(str, kw, kwsz);
		dt_add_property_string(node, "ccin", str);
		cinfo = card_info_lookup(str);
		if (cinfo) {
			dt_add_property_string(node,
				       "description", cinfo->description);
		} else {
			dt_add_property_string(node, "description", "Unknown");
			prlog(PR_WARNING,
			      "VPD: CCIN desc not available for : %s\n", str);
		}
		free(str);
	}
	return;
no_memory:
	prerror("VPD: memory allocation failure in VINI parsing\n");
}

static bool valid_child_entry(const struct slca_entry *entry)
{
	if ((entry->install_indic == SLCA_INSTALL_INSTALLED) &&
		(entry->vpd_collected == SLCA_VPD_COLLECTED))
		return true;

	return false;
}

static void vpd_add_children(struct dt_node *parent, uint16_t slca_index)
{
	const struct slca_entry *s_entry, *child;
	uint16_t current_child_index, max_index;

	s_entry = slca_get_entry(slca_index);
	if (!s_entry || (s_entry->nr_child == 0))
		return;

	/*
	 * This slca_entry has children. Parse the children array
	 * and add nodes for valid entries.
	 *
	 * A child entry is valid if all of the following criteria is met
	 *	a. SLCA_INSTALL_INSTALLED is set in s_entry->install_indic
	 *	b. SLCA_VPD_COLLECTED is set in s_entry->vpd_collected
	 *	c. The SLCA is not a duplicate entry.
	 */

	/* current_index tracks where we are right now in the array */
	current_child_index = be16_to_cpu(s_entry->child_index);

	/* max_index tracks how far down the array we must traverse */
	max_index = be16_to_cpu(s_entry->child_index)
				+ be16_to_cpu(s_entry->nr_child);

	while (current_child_index < max_index) {
		child = slca_get_entry(current_child_index);
		if (!child)
			return;

		if (valid_child_entry(child)) {
			struct dt_node *node;

			node = dt_create_vpd_node(parent, child);
			if (!node)
				return;
		}

		/* Skip dups -- currently we presume dups are contiguous */
		if (child->nr_dups > 0)
			current_child_index += child->nr_dups;
		current_child_index++;
	}
	return;
}

/* Create the vpd node and add its children */
static struct dt_node *dt_create_vpd_node(struct dt_node *parent,
					  const struct slca_entry *entry)
{
	struct dt_node *node;
	const char *name;
	uint64_t addr;

	name = vpd_map_name(entry->fru_id);
	addr = (uint64_t)be16_to_cpu(entry->rsrc_id);
	node = dt_new_addr(parent, name, addr);
	if (!node) {
		prerror("VPD: Creating node at %s@%"PRIx64" failed\n", name, addr);
		return NULL;
	}

	/* Add location code */
	slca_vpd_add_loc_code(node, be16_to_cpu(entry->my_index));
	/* Add FRU label */
	dt_add_property(node, "fru-type", entry->fru_id, 2);
	/* Recursively add children */
	vpd_add_children(node, be16_to_cpu(entry->my_index));

	return node;
}

struct dt_node *dt_add_vpd_node(const struct HDIF_common_hdr *hdr,
				int indx_fru, int indx_vpd)
{
	const struct spira_fru_id *fru_id;
	unsigned int fruvpd_sz, fru_id_sz;
	const struct slca_entry *entry;
	struct dt_node *dt_vpd, *node;
	static bool first = true;
	const void *fruvpd;
	const char *name;
	uint64_t addr;
	char *lname;
	int len;

	fru_id = HDIF_get_idata(hdr, indx_fru, &fru_id_sz);
	if (!fru_id)
		return NULL;

	fruvpd = HDIF_get_idata(hdr, indx_vpd, &fruvpd_sz);
	if (!CHECK_SPPTR(fruvpd))
		return NULL;

	dt_vpd = dt_find_by_path(dt_root, "/vpd");
	if (!dt_vpd)
		return NULL;

	if (first) {
		entry = slca_get_entry(SLCA_ROOT_INDEX);
		if (!entry) {
			prerror("VPD: Could not find the slca root entry\n");
			return NULL;
		}

		node = dt_create_vpd_node(dt_vpd, entry);
		if (!node)
			return NULL;

		first = false;
	}

	entry = slca_get_entry(be16_to_cpu(fru_id->slca_index));
	if (!entry)
		return NULL;

	name = vpd_map_name(entry->fru_id);
	addr = (uint64_t)be16_to_cpu(entry->rsrc_id);
	len = strlen(name) + STR_MAX_CHARS(addr) + 2;
	lname = zalloc(len);
	if (!lname) {
		prerror("VPD: Failed to allocate memory\n");
		return NULL;
	}

	snprintf(lname, len, "%s@%llx", name, (long long)addr);

	/* Get the node already created */
	node = dt_find_by_name(dt_vpd, lname);
	free(lname);
	/*
	 * It is unlikely that node not found because vpd nodes have the
	 * corresponding slca entry which we would have used to populate the vpd
	 * tree during the 'first' pass above so that we just need to perform
	 * VINI parse and add the vpd data..
	 * Still, we consider this case and create fresh node under '/vpd' if
	 * 'node' not found.
	 */
	if (!node) {
		node = dt_create_vpd_node(dt_vpd, entry);
		if (!node)
			return NULL;
	}

	/* Parse VPD fields, ensure that it has not been added already */
	if (vpd_valid(fruvpd, fruvpd_sz)
	    && !dt_find_property(node, "ibm,vpd")) {
		dt_add_property(node, "ibm,vpd", fruvpd, fruvpd_sz);
		vpd_vini_parse(node, fruvpd, fruvpd_sz);
	}

	return node;
}

static void dt_add_model_name(char *model)
{
	const char *model_name = NULL;
	const struct machine_info *mi;
	const struct iplparams_sysparams *p;
	const struct HDIF_common_hdr *iplp;

	iplp = get_hdif(&spira.ntuples.ipl_parms, "IPLPMS");
	if (!iplp)
		goto def_model;

	p = HDIF_get_idata(iplp, IPLPARAMS_SYSPARAMS, NULL);
	if (!CHECK_SPPTR(p))
		goto def_model;

	if (be16_to_cpu(iplp->version) >= 0x60)
		model_name = p->sys_type_str;

def_model:
	if (!model_name || model_name[0] == '\0') {
		mi = machine_info_lookup(model);
		if (mi) {
			model_name = mi->name;
		} else {
			model_name = "Unknown";
			prlog(PR_WARNING, "VPD: Model name %s not known\n", model);
		}
	}

	dt_add_property_string(dt_root, "model-name", model_name);
}

static void vpd_add_property_string(struct dt_node *n, const char *name,
				    const void *vpd, unsigned int sz)
{
	char *str = zalloc(sz + 1);
	if (!str)
		return;
	memcpy(str, vpd, sz);
	dt_add_property_string(n, name, str);
	free(str);
}

static void sysvpd_parse_opp(const void *sysvpd, unsigned int sysvpd_sz)
{
	const char *v;
	uint8_t sz;

	v = vpd_find(sysvpd, sysvpd_sz, "OSYS", "MM", &sz);
	if (v)
		vpd_add_property_string(dt_root, "model", v, sz);
	v = vpd_find(sysvpd, sysvpd_sz, "OSYS", "SS", &sz);
	if (v)
		vpd_add_property_string(dt_root, "system-id", v, sz);
}


static void sysvpd_parse_legacy(const void *sysvpd, unsigned int sysvpd_sz)
{
	const char *model;
	const char *system_id;
	const char *brand;
	char *str;
	uint8_t sz;

	model = vpd_find(sysvpd, sysvpd_sz, "VSYS", "TM", &sz);
	if (model) {
		str = zalloc(sz + 1);
		if (str) {
			memcpy(str, model, sz);
			dt_add_property_string(dt_root, "model", str);
			dt_add_model_name(str);
			free(str);
		}
	} else
		dt_add_property_string(dt_root, "model", "Unknown");

	system_id = vpd_find(sysvpd, sysvpd_sz, "VSYS", "SE", &sz);
	if (system_id)
		vpd_add_property_string(dt_root, "system-id", system_id, sz);
	else
		dt_add_property_string(dt_root, "system-id", "Unknown");

	brand = vpd_find(sysvpd, sysvpd_sz, "VSYS", "BR", &sz);
	if (brand)
		vpd_add_property_string(dt_root, "system-brand", brand, sz);
	else
		dt_add_property_string(dt_root, "brand", "Unknown");
}

static void sysvpd_parse(void)
{
	const void *sysvpd;
	unsigned int sysvpd_sz;
	unsigned int fru_id_sz;
	struct dt_node *dt_vpd;
	const struct spira_fru_id *fru_id;
	struct HDIF_common_hdr *sysvpd_hdr;

	sysvpd_hdr = get_hdif(&spira.ntuples.system_vpd, SYSVPD_HDIF_SIG);
	if (!sysvpd_hdr)
		return;

	fru_id = HDIF_get_idata(sysvpd_hdr, SYSVPD_IDATA_FRU_ID, &fru_id_sz);
	if (!fru_id)
		return;

	sysvpd = HDIF_get_idata(sysvpd_hdr, SYSVPD_IDATA_KW_VPD, &sysvpd_sz);
	if (!CHECK_SPPTR(sysvpd))
		return;

	/* Add system VPD */
	dt_vpd = dt_find_by_path(dt_root, "/vpd");
	if (dt_vpd) {
		dt_add_property(dt_vpd, "ibm,vpd", sysvpd, sysvpd_sz);
		slca_vpd_add_loc_code(dt_vpd, be16_to_cpu(fru_id->slca_index));
	}

	/* Look for the new OpenPower "OSYS" first */
	if (vpd_find_record(sysvpd, sysvpd_sz, "OSYS", NULL))
		sysvpd_parse_opp(sysvpd, sysvpd_sz);
	else
		sysvpd_parse_legacy(sysvpd, sysvpd_sz);
}

static void iokid_vpd_parse(const struct HDIF_common_hdr *iohub_hdr)
{
	const struct HDIF_child_ptr *iokids;
	const struct HDIF_common_hdr *iokid;
	unsigned int i;

	iokids = HDIF_child_arr(iohub_hdr, CECHUB_CHILD_IO_KIDS);
	if (!CHECK_SPPTR(iokids)) {
		prerror("VPD: No IOKID child array\n");
		return;
	}

	for (i = 0; i < be32_to_cpu(iokids->count); i++) {
		iokid = HDIF_child(iohub_hdr, iokids, i,
					IOKID_FRU_HDIF_SIG);
		/* IO KID VPD structure layout is similar to FRUVPD */
		if (iokid)
			dt_add_vpd_node(iokid,
				FRUVPD_IDATA_FRU_ID, FRUVPD_IDATA_KW_VPD);
	}
}

static void iohub_vpd_parse(void)
{
	const struct HDIF_common_hdr *iohub_hdr;
	const struct cechub_hub_fru_id *fru_id_data;
	unsigned int i, vpd_sz, fru_id_sz;

	if (!get_hdif(&spira.ntuples.cec_iohub_fru, CECHUB_FRU_HDIF_SIG)) {
		prerror("VPD: Could not find IO HUB FRU data\n");
		return;
	}

	for_each_ntuple_idx(&spira.ntuples.cec_iohub_fru, iohub_hdr,
					i, CECHUB_FRU_HDIF_SIG) {

		fru_id_data = HDIF_get_idata(iohub_hdr,
					     CECHUB_FRU_ID_DATA_AREA,
					     &fru_id_sz);

		/* P8, IO HUB is on processor card and we have a
		 * daughter card array
		 */
		if (fru_id_data &&
		    be32_to_cpu(fru_id_data->card_type) == CECHUB_FRU_TYPE_CPU_CARD) {
			iokid_vpd_parse(iohub_hdr);
			continue;
		}

		/* On P7, the keyword VPD will not be NULL */
		if (HDIF_get_idata(iohub_hdr,
				   CECHUB_ASCII_KEYWORD_VPD, &vpd_sz))
			dt_add_vpd_node(iohub_hdr, CECHUB_FRU_ID_DATA,
					CECHUB_ASCII_KEYWORD_VPD);
	}
}

static void _vpd_parse(struct spira_ntuple tuple)
{
	const struct HDIF_common_hdr *fruvpd_hdr;
	unsigned int i;

	if (!get_hdif(&tuple, FRUVPD_HDIF_SIG))
		return;

	for_each_ntuple_idx(&tuple, fruvpd_hdr, i, FRUVPD_HDIF_SIG)
		dt_add_vpd_node(fruvpd_hdr,
				FRUVPD_IDATA_FRU_ID, FRUVPD_IDATA_KW_VPD);
}

void vpd_parse(void)
{
	const struct HDIF_common_hdr *fruvpd_hdr;

	/* Enclosure */
	_vpd_parse(spira.ntuples.nt_enclosure_vpd);

	/* Backplane */
	_vpd_parse(spira.ntuples.backplane_vpd);

	/* System VPD uses the VSYS record, so its special */
	sysvpd_parse();

	/* clock card -- does this use the FRUVPD sig? */
	_vpd_parse(spira.ntuples.clock_vpd);

	/* Anchor card */
	_vpd_parse(spira.ntuples.anchor_vpd);

	/* Op panel -- does this use the FRUVPD sig? */
	_vpd_parse(spira.ntuples.op_panel_vpd);

	/* External cache FRU vpd -- does this use the FRUVPD sig? */
	_vpd_parse(spira.ntuples.ext_cache_fru_vpd);

	/* Misc CEC FRU */
	_vpd_parse(spira.ntuples.misc_cec_fru_vpd);

	/* CEC IO HUB FRU */
	iohub_vpd_parse();

	/*
	 * SP subsystem -- while the rest of the SPINFO structure is
	 * different, the FRU ID data and pointer pair to keyword VPD
	 * are the same offset as a FRUVPD entry. So reuse it
	 */
	fruvpd_hdr = get_hdif(&spira.ntuples.sp_subsys, SPSS_HDIF_SIG);
	if (fruvpd_hdr)
		dt_add_vpd_node(fruvpd_hdr,
				FRUVPD_IDATA_FRU_ID, FRUVPD_IDATA_KW_VPD);
}