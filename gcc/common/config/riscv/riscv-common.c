/* Common hooks for RISC-V.
   Copyright (C) 2016-2020 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include <sstream>
#include <vector>

#define INCLUDE_STRING
#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "common/common-target.h"
#include "common/common-target-def.h"
#include "opts.h"
#include "flags.h"
#include "diagnostic-core.h"
#include "config/riscv/riscv-protos.h"
#include "config/riscv/riscv-subset.h"

/* Type for implied ISA info.  */
struct riscv_implied_info_t
{
  const char *ext;
  const char *implied_ext;
};

/* Implied ISA info, must end with NULL sentinel.  */
riscv_implied_info_t riscv_implied_info[] =
{
  {"d", "f"},
  /* XXX: Work-around, base vector extension zve64d + load/store zvl128b.  */
  {"v", "zvl128b"},
  {"v", "zve64d"},

  {"zve32f", "f"},
  {"zve64f", "f"},
  {"zve64d", "d"},

  {"zve32x", "zvl32b"},
  {"zve32f", "zve32x"},
  {"zve32f", "zvl32b"},

  {"zve64x", "zve32x"},
  {"zve64x", "zvl64b"},
  {"zve64f", "zve32f"},
  {"zve64f", "zve64x"},
  {"zve64f", "zvl64b"},
  {"zve64d", "zve64f"},
  {"zve64d", "zvl64b"},

  {"zvl64b", "zvl32b"},
  {"zvl128b", "zvl64b"},
  {"zvl256b", "zvl128b"},
  {"zvl512b", "zvl256b"},
  {"zvl1024b", "zvl512b"},
  {"zvl2048b", "zvl1024b"},
  {"zvl4096b", "zvl2048b"},
  {"zvl8192b", "zvl4096b"},
  {"zvl16384b", "zvl8192b"},
  {"zvl32768b", "zvl16384b"},
  {"zvl65536b", "zvl32768b"},

  /* XXX: Work-around, zvqmac need v, and v implied zve64d and zvl128b.  */
  {"zvqmac", "v"},
  {"zvqmac", "zve64d"},
  {"zvqmac", "zvl28b"},

  {"f", "zicsr"},
  {"d", "zicsr"},
  {"b", "zbb"},
  {"b", "zbs"},
  {"b", "zbc"},
  {"b", "zba"},

  {"zcf",  "zca"},
  {"zcd",  "zca"},
  {"zcb",  "zca"},

  {"zvfbfmin", "zfbfmin"},
  {"zvfbfwma", "zfbfmin"},
  {"zvfbfwma", "zvfbfmin"},

  {NULL, NULL}
};

/* This structure holds version information for specific ISA version.  */

struct riscv_ext_version
{
  const char *name;
  enum riscv_isa_spec_class isa_spec_class;
  int major_version;
  int minor_version;
};

/* All standard extensions defined in all supported ISA spec.  */
static const struct riscv_ext_version riscv_ext_version_table[] =
{
  /* name, ISA spec, major version, minor_version.  */
  {"e", ISA_SPEC_CLASS_20191213, 1, 9},
  {"e", ISA_SPEC_CLASS_20190608, 1, 9},
  {"e", ISA_SPEC_CLASS_2P2,      1, 9},

  {"i", ISA_SPEC_CLASS_20191213, 2, 1},
  {"i", ISA_SPEC_CLASS_20190608, 2, 1},
  {"i", ISA_SPEC_CLASS_2P2,      2, 0},

  {"m", ISA_SPEC_CLASS_20191213, 2, 0},
  {"m", ISA_SPEC_CLASS_20190608, 2, 0},
  {"m", ISA_SPEC_CLASS_2P2,      2, 0},

  {"a", ISA_SPEC_CLASS_20191213, 2, 1},
  {"a", ISA_SPEC_CLASS_20190608, 2, 0},
  {"a", ISA_SPEC_CLASS_2P2,      2, 0},

  {"f", ISA_SPEC_CLASS_20191213, 2, 2},
  {"f", ISA_SPEC_CLASS_20190608, 2, 2},
  {"f", ISA_SPEC_CLASS_2P2,      2, 0},

  {"d", ISA_SPEC_CLASS_20191213, 2, 2},
  {"d", ISA_SPEC_CLASS_20190608, 2, 2},
  {"d", ISA_SPEC_CLASS_2P2,      2, 0},

  {"c", ISA_SPEC_CLASS_20191213, 2, 0},
  {"c", ISA_SPEC_CLASS_20190608, 2, 0},
  {"c", ISA_SPEC_CLASS_2P2,      2, 0},

  {"zca", ISA_SPEC_CLASS_NONE, 1, 0},
  {"zcb", ISA_SPEC_CLASS_NONE, 1, 0},
  {"zcd", ISA_SPEC_CLASS_NONE, 1, 0},
  {"zcf", ISA_SPEC_CLASS_NONE, 1, 0},

  {"p", ISA_SPEC_CLASS_20191213, 0, 2},
  {"p", ISA_SPEC_CLASS_20190608, 0, 2},
  {"p", ISA_SPEC_CLASS_2P2,      0, 9},

  {"zpn", ISA_SPEC_CLASS_2P2,      0, 9},
  {"zprvsfextra", ISA_SPEC_CLASS_2P2,      0, 9},
  {"zpsfoperand", ISA_SPEC_CLASS_2P2,      0, 9},

  {"zicond",   ISA_SPEC_CLASS_NONE, 1, 0},
  {"zicsr", ISA_SPEC_CLASS_20191213, 2, 0},
  {"zicsr", ISA_SPEC_CLASS_20190608, 2, 0},

  {"zihintpause", ISA_SPEC_CLASS_NONE, 1, 0},
  {"zifencei", ISA_SPEC_CLASS_20191213, 2, 0},
  {"zifencei", ISA_SPEC_CLASS_20190608, 2, 0},

  {"zfh",     ISA_SPEC_CLASS_NONE, 1, 0},

  {"zve32x", ISA_SPEC_CLASS_NONE, 1, 0},
  {"zve32f", ISA_SPEC_CLASS_NONE, 1, 0},
  {"zve32d", ISA_SPEC_CLASS_NONE, 1, 0},
  {"zve64x", ISA_SPEC_CLASS_NONE, 1, 0},
  {"zve64f", ISA_SPEC_CLASS_NONE, 1, 0},
  {"zve64d", ISA_SPEC_CLASS_NONE, 1, 0},

  {"zvl32b", ISA_SPEC_CLASS_NONE, 1, 0},
  {"zvl64b", ISA_SPEC_CLASS_NONE, 1, 0},
  {"zvl128b", ISA_SPEC_CLASS_NONE, 1, 0},
  {"zvl256b", ISA_SPEC_CLASS_NONE, 1, 0},
  {"zvl512b", ISA_SPEC_CLASS_NONE, 1, 0},
  {"zvl1024b", ISA_SPEC_CLASS_NONE, 1, 0},
  {"zvl2048b", ISA_SPEC_CLASS_NONE, 1, 0},
  {"zvl4096b", ISA_SPEC_CLASS_NONE, 1, 0},
  {"zvl8192b", ISA_SPEC_CLASS_NONE, 1, 0},
  {"zvl16384b", ISA_SPEC_CLASS_NONE, 1, 0},
  {"zvl32768b", ISA_SPEC_CLASS_NONE, 1, 0},
  {"zvl65536b", ISA_SPEC_CLASS_NONE, 1, 0},
  {"zfbfmin", ISA_SPEC_CLASS_NONE, 0, 8},
  {"zvfbfmin", ISA_SPEC_CLASS_NONE, 0, 8},
  {"zvfbfwma", ISA_SPEC_CLASS_NONE, 0, 8},

  {"zfa",     ISA_SPEC_CLASS_NONE, 0, 1},

  {"xtheadc", ISA_SPEC_CLASS_NONE, 2, 0},
  {"xtheade", ISA_SPEC_CLASS_NONE, 2, 0},
  {"xtheadse", ISA_SPEC_CLASS_NONE, 2, 0},
  {"xtheadvdot", ISA_SPEC_CLASS_NONE, 1, 0},
  {"xtheadmatrix", ISA_SPEC_CLASS_NONE, 0, 3},

  {"v", ISA_SPEC_CLASS_NONE, 1, 0},

  {"b",   ISA_SPEC_CLASS_NONE, 1, 0},
  {"zba", ISA_SPEC_CLASS_NONE, 1, 0},
  {"zbb", ISA_SPEC_CLASS_NONE, 1, 0},
  {"zbc", ISA_SPEC_CLASS_NONE, 1, 0},
  {"zbs", ISA_SPEC_CLASS_NONE, 1, 0},

  /* Terminate the list.  */
  {NULL, ISA_SPEC_CLASS_NONE, 0, 0}
};

static const riscv_cpu_info riscv_cpu_tables[] =
{
#define RISCV_CORE(CORE_NAME, ARCH, TUNE) \
    {CORE_NAME, ARCH, TUNE},
#include "../../../config/riscv/riscv-cores.def"
    {NULL, NULL, NULL}
};

static const char *riscv_supported_std_ext (void);

static riscv_subset_list *current_subset_list = NULL;

const riscv_subset_list *riscv_current_subset_list ()
{
  return current_subset_list;
}

/* struct for recording multi-lib info.  */
struct riscv_multi_lib_info_t {
  std::string path;
  std::string arch_str;
  std::string abi_str;
  std::vector<std::string> conds;
  riscv_subset_list *subset_list;

  static bool parse (struct riscv_multi_lib_info_t *,
		     const std::string &,
		     const std::vector<std::string> &);
};

/* Flag for checking if there is no suitable multi-lib found.  */
static bool riscv_no_matched_multi_lib;

/* Used for record value of -march and -mabi.  */
static std::string riscv_current_arch_str;
static std::string riscv_current_abi_str;

riscv_subset_t::riscv_subset_t ()
  : name (), major_version (0), minor_version (0), next (NULL),
    explicit_version_p (false), implied_p (false)
{
}

riscv_subset_list::riscv_subset_list (const char *arch, location_t loc)
  : m_arch (arch), m_loc (loc), m_head (NULL), m_tail (NULL), m_xlen (0)
{
}

riscv_subset_list::~riscv_subset_list ()
{
  if (!m_head)
    return;

  riscv_subset_t *item = this->m_head;
  while (item != NULL)
    {
      riscv_subset_t *next = item->next;
      delete item;
      item = next;
    }
}

/* Compute the match score of two arch string, return -1 if incompatible.  */
int
riscv_subset_list::match_score (riscv_subset_list *list) const
{
  riscv_subset_t *s;
  int score = 0;
  bool has_a_ext, list_has_a_ext;

  /* Impossible to match if XLEN is different.  */
  if (list->m_xlen != this->m_xlen)
    return -1;

  /* There is different code gen in libstdc++ and libatomic between w/ A-ext
     and w/o A-ext, and it not work if using soft and hard atomic mechanism
     at same time, so they are incompatible.  */
  has_a_ext = this->lookup ("a") != NULL;
  list_has_a_ext = list->lookup ("a") != NULL;

  if (has_a_ext != list_has_a_ext)
    return -1;


  /* list must be subset of current this list, otherwise it not safe to
     link.
     TODO: We might give different weight for each extension, but the rule could
	   be complicated.
     TODO: We might consider the version of each extension.  */
  for (s = list->m_head; s != NULL; s = s->next)
  {
    if (this->lookup (s->name.c_str (), s->major_version, s->minor_version) == NULL)
      {
	/* TODO: c == zca_zcd_zcf in rv32, c == zca_zcd in rv64.  */
	if (s->name.compare ("c") == 0
	    && this->lookup ("zca") != NULL
	    && this->lookup ("zcd") != NULL)
	  {
	    if (this->m_xlen  == 32)
	      score += (this->lookup ("zcf") != NULL) ? 3 : 0;
	    else
	      score += 2;
	    continue;
	  }
	return -1;
      }
    else
      score++;
  }

  return (score != 0) ? score : -1;
}

/* Get the rank for single-letter subsets, lower value meaning higher
   priority.  */

static int
single_letter_subset_rank (char ext)
{
  int rank;

  switch (ext)
    {
    case 'i':
      return 0;
    case 'e':
      return 1;
    default:
      break;
    }

  const char *all_ext = riscv_supported_std_ext ();
  const char *ext_pos = strchr (all_ext, ext);
  if (ext_pos == NULL)
    /* If got an unknown extension letter, then give it an alphabetical
       order, but after all known standard extension.  */
    rank = strlen (all_ext) + ext - 'a';
  else
    rank = (int)(ext_pos - all_ext) + 2 /* e and i has higher rank.  */;

  return rank;
}

/* Get the rank for multi-letter subsets, lower value meaning higher
   priority.  */

static int
multi_letter_subset_rank (const std::string &subset)
{
  gcc_assert (subset.length () >= 2);
  int high_order = -1;
  int low_order = 0;
  /* The order between multi-char extensions: s -> h -> z -> x.  */
  char multiletter_class = subset[0];
  switch (multiletter_class)
    {
    case 'h':
      high_order = 0;
      break;
    case 'z':
      gcc_assert (subset.length () > 2);
      high_order = 1;
      break;
    case 's':
      high_order = 2;
      break;
    case 'x':
      high_order = 3;
      break;
    default:
      gcc_unreachable ();
      return -1;
    }

  if (multiletter_class == 'z')
    /* Order for z extension on spec: If multiple "Z" extensions are named, they
       should be ordered first by category, then alphabetically within a
       category - for example, "Zicsr_Zifencei_Zam". */
    low_order = single_letter_subset_rank (subset[1]);
  else
    low_order = 0;

  return (high_order << 8) + low_order;
}

/* subset compare

  Returns an integral value indicating the relationship between the subsets:
  Return value  indicates
  -1            B has higher order than A.
  0             A and B are same subset.
  1             A has higher order than B.

*/

static int
subset_cmp (const std::string &a, const std::string &b)
{
  if (a == b)
    return 0;

  size_t a_len = a.length ();
  size_t b_len = b.length ();

  /* Single-letter extension always get higher order than
     multi-letter extension.  */
  if (a_len == 1 && b_len != 1)
    return 1;

  if (a_len != 1 && b_len == 1)
    return -1;

  if (a_len == 1 && b_len == 1)
    {
      int rank_a = single_letter_subset_rank (a[0]);
      int rank_b = single_letter_subset_rank (b[0]);

      if (rank_a < rank_b)
	return 1;
      else
	return -1;
    }
  else
    {
      int rank_a = multi_letter_subset_rank(a);
      int rank_b = multi_letter_subset_rank(b);

      /* Using alphabetical/lexicographical order if they have same rank.  */
      if (rank_a == rank_b)
	/* The return value of strcmp has opposite meaning.  */
	return -strcmp (a.c_str (), b.c_str ());
      else
	return (rank_a < rank_b) ? 1 : -1;
    }
}

/* Add new subset to list.  */

void
riscv_subset_list::add (const char *subset, int major_version,
			int minor_version, bool explicit_version_p,
			bool implied_p)
{
  riscv_subset_t *ext = lookup (subset);

  if (ext)
    {
      if (ext->implied_p)
	{
	  /* We won't add impiled `ext` if it already in list. */
	  gcc_assert (!implied_p);
	  ext->implied_p = implied_p;
	  ext->major_version = major_version;
	  ext->minor_version = minor_version;
	}
      else
	error_at (
	  m_loc,
	  "%<-march=%s%>: Extension `%s' appear more than one time.",
	  m_arch,
	  subset);

      return;
    }

  riscv_subset_t *s = new riscv_subset_t ();
  riscv_subset_t *itr;

  if (m_head == NULL)
    m_head = s;

  s->name = subset;
  s->major_version = major_version;
  s->minor_version = minor_version;
  s->explicit_version_p = explicit_version_p;
  s->implied_p = implied_p;
  s->next = NULL;

  if (m_tail == NULL)
    {
      m_tail = s;
      return;
    }

  /* e, i or g should be first subext, never come here.  */
  gcc_assert (subset[0] != 'e'
	      && subset[0] != 'i'
	      && subset[0] != 'g');

  if (m_tail == m_head)
    {
      gcc_assert (m_head->next == NULL);
      m_head->next = s;
      m_tail = s;
      return;
    }

  gcc_assert (m_head->next != NULL);

  /* Subset list must in canonical order, but implied subset won't
     add in canonical order.  */
  for (itr = m_head; itr->next != NULL; itr = itr->next)
    {
      riscv_subset_t *next = itr->next;
      int cmp = subset_cmp (s->name, next->name);
      gcc_assert (cmp != 0);

      if (cmp > 0)
	{
	  s->next = next;
	  itr->next = s;
	  return;
	}
    }

  /* Insert at tail of the list.  */
  itr->next = s;
  m_tail = s;
}

static void
get_default_version (const char *ext,
		     unsigned int *major_version,
		     unsigned int *minor_version)
{
  const riscv_ext_version *ext_ver;
  for (ext_ver = &riscv_ext_version_table[0];
       ext_ver->name != NULL;
       ++ext_ver)
    if (strcmp (ext, ext_ver->name) == 0)
      {
	if ((ext_ver->isa_spec_class == riscv_isa_spec) ||
	    (ext_ver->isa_spec_class == ISA_SPEC_CLASS_NONE))
	  {
	    *major_version = ext_ver->major_version;
	    *minor_version = ext_ver->minor_version;
	    return;
	  }
      }

  /* Not found version info.  */
  *major_version = 0;
  *minor_version = 0;
}

/* Add new subset to list, but using default version from ISA spec version.  */

void
riscv_subset_list::add (const char *subset, bool implied_p)
{
  unsigned int major_version = 0, minor_version = 0;

  get_default_version (subset, &major_version, &minor_version);

  add (subset, major_version, minor_version, false, implied_p);
}

/* Convert subset info to string with explicit version info,
   VERSION_P to determine append version info or not.  */

std::string
riscv_subset_list::to_string (bool version_p) const
{
  std::ostringstream oss;
  oss << "rv" << m_xlen;

  bool first = true;
  bool underline = false;
  riscv_subset_t *subset;

  bool skip_zifencei = false;
  bool skip_zicsr = false;

  /* For RISC-V ISA version 2.2 or earlier version, zicsr and zifencei is
     included in the base ISA.  */
  if (riscv_isa_spec == ISA_SPEC_CLASS_2P2)
    {
      skip_zifencei = true;
      skip_zicsr = true;
    }

#ifndef HAVE_AS_MISA_SPEC
  /* Skip since older binutils doesn't recognize zicsr.  */
  skip_zicsr = true;
#endif
#ifndef HAVE_AS_MARCH_ZIFENCE
  /* Skip since older binutils doesn't recognize zifencei, we made
     a mistake in that binutils 2.35 supports zicsr but not zifencei.  */
  skip_zifencei = true;
#endif

  for (subset = m_head; subset != NULL; subset = subset->next)
    {
      if (subset->implied_p && skip_zifencei && subset->name == "zifencei")
	continue;

      if (subset->implied_p && skip_zicsr && subset->name == "zicsr")
	continue;

      /* For !version_p, we only separate extension with underline for
	 multi-letter extension.  */
      if (version_p
	  || subset->explicit_version_p
	  || subset->name.length() > 1)
	{
	  underline = true;
	  if (!first)
	    oss << '_';
	}
      else if (underline)
	{
	  underline = false;
	  oss << '_';
	}

      first = false;

      oss << subset->name;

      /* Let binutils decide the extension version if we don't know.  */
      if ((version_p || subset->explicit_version_p) &&
	  (subset->major_version != 0 || subset->minor_version != 0))
	oss  << subset->major_version
	     << 'p'
	     << subset->minor_version;
    }

  return oss.str ();
}

/* Find subset in list with version checking, return NULL if not found.
   major/minor version checking can be ignored if major_version/minor_version
   is RISCV_DONT_CARE_VERSION.  */

riscv_subset_t *
riscv_subset_list::lookup (const char *subset, int major_version,
			   int minor_version) const
{
  riscv_subset_t *s;

  for (s = m_head; s != NULL; s = s->next)
    if (strcasecmp (s->name.c_str (), subset) == 0)
      {
	if ((major_version != RISCV_DONT_CARE_VERSION)
	    && (s->major_version != major_version))
	  return NULL;

	if ((minor_version != RISCV_DONT_CARE_VERSION)
	    && (s->minor_version != minor_version))
	  return NULL;

	return s;
      }

  return s;
}

/* Return string which contains all supported standard extensions in
   canonical order.  */

static const char *
riscv_supported_std_ext (void)
{
  return "mafdqlcbjtpvn";
}

/* Parsing subset version.

   Return Value:
     Points to the end of version

   Arguments:
     `ext`: This extension.
     `p`: Current parsing position.
     `major_version`: Parsing result of major version, using
      default_major_version if version is not present in arch string.
     `minor_version`: Parsing result of minor version, set to 0 if version is
     not present in arch string, but set to `default_minor_version` if
     `major_version` using default_major_version.
     `std_ext_p`: True if parsing std extension.
     `explicit_version_p`: True if this subset is not using default version.  */

const char *
riscv_subset_list::parsing_subset_version (const char *ext,
					   const char *p,
					   unsigned *major_version,
					   unsigned *minor_version,
					   bool std_ext_p,
					   bool *explicit_version_p)
{
  bool major_p = true;
  unsigned version = 0;
  unsigned major = 0;
  unsigned minor = 0;
  *explicit_version_p = false;

  /* If we got `p`, that means we are still parsing standard extension.  */
  gcc_assert (std_ext_p || *p != 'p');

  if (*p != 'p') {
    for (; *p; ++p)
      {
	if (*p == 'p')
	  {
	    if (!ISDIGIT (*(p+1)))
	      {
		error_at (m_loc, "%<-march=%s%>: Expect number "
			  "after %<%dp%>.", m_arch, version);
		return NULL;
	      }
	    if (!major_p)
	      {
		error_at (m_loc, "%<-march=%s%>: ISA version with more than 2 "
			  "level is not supported.", m_arch);
		return NULL;
	      }
	    major = version;
	    major_p = false;
	    version = 0;
	  }
	else if (ISDIGIT (*p))
	  version = (version * 10) + (*p - '0');
	else
	  break;
      }
  }

  if (major_p)
    major = version;
  else
    minor = version;

  if (major == 0 && minor == 0)
    get_default_version (ext, major_version, minor_version);
  else
    {
      *explicit_version_p = true;
      *major_version = major;
      *minor_version = minor;
    }
  return p;
}

/* Parsing function for standard extensions.

   Return Value:
     Points to the end of extensions.

   Arguments:
     `p`: Current parsing position.  */

const char *
riscv_subset_list::parse_std_ext (const char *p)
{
  const char *all_std_exts = riscv_supported_std_ext ();
  const char *std_exts = all_std_exts;

  unsigned major_version = 0;
  unsigned minor_version = 0;
  char std_ext = '\0';
  bool explicit_version_p = false;

  /* First letter must start with i, e or g.  */
  switch (*p)
    {
    case 'i':
      p++;
      p = parsing_subset_version ("i", p, &major_version, &minor_version,
				  /* std_ext_p= */ true, &explicit_version_p);
      add ("i", major_version, minor_version, explicit_version_p, false);
      break;

    case 'e':
      p++;
      p = parsing_subset_version ("e", p, &major_version, &minor_version,
				  /* std_ext_p= */ true, &explicit_version_p);

      add ("e", major_version, minor_version, explicit_version_p, false);

      if (m_xlen > 32)
	{
	  error_at (m_loc, "%<-march=%s%>: rv%de is not a valid base ISA",
		    m_arch, m_xlen);
	  return NULL;
	}
      break;

    case 'g':
      p++;
      p = parsing_subset_version ("g", p, &major_version, &minor_version,
				  /* std_ext_p= */ true, &explicit_version_p);
      if (major_version != 0 || minor_version != 0)
	{
	  warning_at (m_loc, 0, "version of `g` will be omitted, please "
				"specify version for individual extension.");
	}

      /* We have special rule for G, we disallow rv32gm2p but allow rv32g_zicsr
	 here, basically we treating G expand to imafd and implied zicsr and
	 zifencei.  */

      add ("i", false);
      add ("m", false);
      add ("a", false);
      add ("f", false);
      add ("d", false);
      add ("zicsr", true);
      add ("zifencei", true);

      break;

    default:
      error_at (m_loc, "%<-march=%s%>: first ISA subset must be %<e%>, "
		"%<i%> or %<g%>", m_arch);
      return NULL;
    }

  while (p != NULL && *p)
    {
      char subset[2] = {0, 0};

      if (*p == 'x' || *p == 's' || *p == 'h' || *p == 'z')
	break;

      if (*p == '_')
	{
	  p++;
	  continue;
	}

      std_ext = *p;

      /* Checking canonical order.  */
      while (*std_exts && std_ext != *std_exts)
	std_exts++;

      if (std_ext != *std_exts)
	{
	  if (strchr (all_std_exts, std_ext) == NULL)
	    error_at (m_loc, "%<-march=%s%>: unsupported ISA subset %<%c%>",
		      m_arch, *p);
	  else
	    error_at (m_loc,
		      "%<-march=%s%>: ISA string is not in canonical order. "
		      "%<%c%>", m_arch, *p);
	  return NULL;
	}

      std_exts++;

      p++;
      subset[0] = std_ext;

      p = parsing_subset_version (subset, p, &major_version, &minor_version,
				  /* std_ext_p= */ true, &explicit_version_p);

      add (subset, major_version, minor_version, explicit_version_p, false);
    }
  return p;
}


/* Check any implied extensions for EXT.  */
void
riscv_subset_list::handle_implied_ext (riscv_subset_t *ext)
{
  riscv_implied_info_t *implied_info;
  for (implied_info = &riscv_implied_info[0];
	implied_info->ext;
	++implied_info)
    {
      if (strcmp (ext->name.c_str (), implied_info->ext) != 0)
	continue;

      /* Skip if implied extension already present.  */
      if (lookup (implied_info->implied_ext))
	continue;
      /* If need imply(currently check extension sub-version much than 7).  */
      if(ext->minor_version > 7)
      {
	if (ext->explicit_version_p)
	/* Version of implied extension will get from current ISA spec
	   version.  */
	  add (implied_info->implied_ext, ext->major_version, ext->minor_version,
		  ext->explicit_version_p, true);
	else
	/* Version of implied extension will get its default version.  */
	  add (implied_info->implied_ext, true);
      }
      else
	continue;
    }

  /* For RISC-V ISA version 2.2 or earlier version, zicsr and zifence is
     included in the base ISA.  */
  if (riscv_isa_spec == ISA_SPEC_CLASS_2P2)
    {
      if (lookup ("zicsr") == NULL)
	add ("zicsr", true);

      if (lookup ("zifencei") == NULL)
	add ("zifencei", true);
    }
}

/*  Do some work for Thead specific extensions.  */

void
riscv_subset_list::thead_specific_work ()
{
  riscv_subset_t *implied_subset;

  /* Compatible with X-Thead old extension instruction names.  */
  if ((implied_subset = lookup ("xthead")))
    {
      if (m_xlen == 32)
	 if (lookup ("e"))
	   implied_subset->name = "xtheadse";
	 else
	   implied_subset->name = "xtheade";
      else if (m_xlen == 64)
	 implied_subset->name = "xtheadc";

      if (implied_subset->major_version == 0 && implied_subset->minor_version == 0)
	get_default_version (implied_subset->name.c_str(),
			    (unsigned int *)&implied_subset->major_version,
			    (unsigned int *)&implied_subset->minor_version);
  }

  if ((implied_subset = lookup ("v", 0, 7)))
    {
      if (!lookup ("xtheadzvamo"))
	add ("xtheadzvamo", implied_subset->major_version,
	     implied_subset->minor_version,
	     implied_subset->explicit_version_p, false);
    }

  if (lookup ("xcki"))
    error_at (m_loc, "The ISA name 'xcki' is no longer maintained, please use"
		     " 'xthead{c,e,se}' instead");

  if (lookup ("xcrypto"))
    error_at (m_loc, "The ISA name 'xcrypto' is no longer maintained, please"
		     " use 'xtheade' instead");

  if ((implied_subset = lookup ("p")))
    {
      if (!lookup ("zpn"))
	add ("zpn", implied_subset->major_version,
	     implied_subset->minor_version,
	     implied_subset->explicit_version_p, false);
      if (!lookup ("zpsfoperand") && m_xlen == 64)
	add ("zpsfoperand", implied_subset->major_version,
	     implied_subset->minor_version,
	     implied_subset->explicit_version_p, false);
      if (!lookup ("zprvsfextra") && m_xlen == 64)
	add ("zprvsfextra", implied_subset->major_version,
	     implied_subset->minor_version,
	     implied_subset->explicit_version_p, false);
    }

  if (m_xlen == 32 && lookup ("zpsfoperand") && !lookup ("zpn"))
    error_at (m_loc, "%<-march=%s%>: The ISA 'zpsfoperand' need 'zpn'.");

  if (m_xlen == 32 && lookup ("zprvsfextra"))
    error_at (m_loc, "%<-march=%s%>: The ISA 'zprvsfextra' need RV64.");

  if (m_xlen == 64
      && (lookup ("zpn") || lookup ("zpsfoperand") || lookup ("zprvsfextra"))
      && !(lookup ("zpn") && lookup ("zpsfoperand") && lookup ("zprvsfextra")))
    error_at (m_loc, "%<-march=%s%>: At RV64, the ISA 'zpn' 'zpsfoperand'"
		     " 'zprvsfextra' must be present or absent at the same"
		     " time.");

}


/* Parsing function for multi-letter extensions.

   Return Value:
     Points to the end of extensions.

   Arguments:
     `p`: Current parsing position.
     `ext_type`: What kind of extensions, 's', 'h', 'z' or 'x'.
     `ext_type_str`: Full name for kind of extension.  */

const char *
riscv_subset_list::parse_multiletter_ext (const char *p,
					  const char *ext_type,
					  const char *ext_type_str)
{
  unsigned major_version = 0;
  unsigned minor_version = 0;
  size_t ext_type_len = strlen (ext_type);

  while (*p)
    {
      if (*p == '_')
	{
	  p++;
	  continue;
	}

      if (strncmp (p, ext_type, ext_type_len) != 0)
	break;

      char *subset = xstrdup (p);
      char *q = subset;
      const char *end_of_version;
      bool explicit_version_p = false;
      char *ext;
      char backup;

      while (*++q != '\0' && *q != '_' && !ISDIGIT (*q))
	;

      backup = *q;
      *q = '\0';
      ext = xstrdup (subset);
      *q = backup;

      end_of_version
	= parsing_subset_version (ext, q, &major_version, &minor_version,
				  /* std_ext_p= */ false, &explicit_version_p);
      free (ext);

      if (end_of_version == NULL)
	return NULL;

      *q = '\0';

      if (strlen (subset) == 1)
	{
	  error_at (m_loc, "%<-march=%s%>: name of %s must be more than 1 letter",
		    m_arch, ext_type_str);
	  free (subset);
	  return NULL;
	}

      add (subset, major_version, minor_version, explicit_version_p, false);
      p += end_of_version - subset;
      free (subset);

      if (*p != '\0' && *p != '_')
	{
	  error_at (m_loc, "%<-march=%s%>: %s must separate with _",
		    m_arch, ext_type_str);
	  return NULL;
	}
    }

  return p;
}

/* Parsing arch string to subset list, return NULL if parsing failed.  */

riscv_subset_list *
riscv_subset_list::parse (const char *arch, location_t loc)
{
  riscv_subset_list *subset_list = new riscv_subset_list (arch, loc);
  riscv_subset_t *itr;
  const char *p = arch;
  if (strncmp (p, "rv32", 4) == 0)
    {
      subset_list->m_xlen = 32;
      p += 4;
    }
  else if (strncmp (p, "rv64", 4) == 0)
    {
      subset_list->m_xlen = 64;
      p += 4;
    }
  else
    {
      error_at (loc, "%<-march=%s%>: ISA string must begin with rv32 or rv64",
		arch);
      goto fail;
    }

  /* Parsing standard extension.  */
  p = subset_list->parse_std_ext (p);

  if (p == NULL)
    goto fail;

  /* Parsing hypervisor extension.  */
  p = subset_list->parse_multiletter_ext (p, "h", "hypervisor extension");

  if (p == NULL)
    goto fail;

  /* Parsing sub-extensions.  */
  p = subset_list->parse_multiletter_ext (p, "z", "sub-extension");

  if (p == NULL)
    goto fail;

  /* Parsing supervisor extension.  */
  p = subset_list->parse_multiletter_ext (p, "s", "supervisor extension");

  if (p == NULL)
    goto fail;

  /* Parsing non-standard extension.  */
  p = subset_list->parse_multiletter_ext (p, "x", "non-standard extension");

  if (p == NULL)
    goto fail;

  if (*p != '\0')
    {
      error_at (loc, "%<-march=%s%>: unexpected ISA string at end: %qs",
		arch, p);
      goto fail;
    }

  for (itr = subset_list->m_head; itr != NULL; itr = itr->next)
    {
      subset_list->handle_implied_ext (itr);
    }

   subset_list->thead_specific_work();

  return subset_list;

fail:
  delete subset_list;
  return NULL;
}

/* Return the current arch string.  */

std::string
riscv_arch_str (bool version_p)
{
  if (current_subset_list)
    return current_subset_list->to_string (version_p);
  else
    return std::string();
}

/* Type for pointer to member of gcc_options.  */
typedef int (gcc_options::*opt_var_ref_t);

/* Types for recording extension to internal flag.  */
struct riscv_ext_flag_table_t {
  const char *ext;
  opt_var_ref_t var_ref;
  int mask;
};

/* Mapping table between extension to internal flag.  */
static const riscv_ext_flag_table_t riscv_ext_flag_table[] =
{
  {"e", &gcc_options::x_target_flags, MASK_RVE},
  {"m", &gcc_options::x_target_flags, MASK_MUL},
  {"a", &gcc_options::x_target_flags, MASK_ATOMIC},
  {"f", &gcc_options::x_target_flags, MASK_HARD_FLOAT},
  {"d", &gcc_options::x_target_flags, MASK_DOUBLE_FLOAT},
  {"c", &gcc_options::x_target_flags, MASK_RVC},
  {"v", &gcc_options::x_target_flags, MASK_VECTOR},
  {"zfh", &gcc_options::x_target_flags, MASK_RVZFH},
  {"zfbfmin", &gcc_options::x_target_flags, MASK_ZFBFMIN},
  {"zvfbfmin", &gcc_options::x_target_flags, MASK_ZVFBFMIN},
  {"zvfbfwma", &gcc_options::x_target_flags, MASK_ZVFBFWMA},
  {"zfa", &gcc_options::x_target_flags, MASK_RVZFA},

  {"xtheadc", &gcc_options::x_target_flags, MASK_XTHEAD_C | MASK_XTHEAD},
  {"xtheade", &gcc_options::x_target_flags, MASK_XTHEAD_E | MASK_XTHEAD},
  {"xtheadse", &gcc_options::x_target_flags, MASK_XTHEAD_SE |MASK_XTHEAD},
  {"xtheadvdot", &gcc_options::x_target_flags, MASK_XTHEAD_V |MASK_XTHEAD},
  {"xtheadmatrix", &gcc_options::x_target_flags, MASK_MATRIX | MASK_XTHEAD},
  {"p", &gcc_options::x_target_flags, MASK_DSP},
  {"zpn", &gcc_options::x_target_flags, MASK_ZPN},
  {"zpsfoperand", &gcc_options::x_target_flags, MASK_ZPSFOPERAND},
  {"zprvsfextra", &gcc_options::x_target_flags, MASK_ZPRVSFEXTRA},

  {"b", &gcc_options::x_target_flags, MASK_BITMANIP},

  {"zicsr",    &gcc_options::x_riscv_zi_subext, MASK_ZICSR},
  {"zifencei", &gcc_options::x_riscv_zi_subext, MASK_ZIFENCEI},

  {"zca",     &gcc_options::x_riscv_zc_subext, MASK_ZCA},
  {"zcb",     &gcc_options::x_riscv_zc_subext, MASK_ZCB},
  {"zcf",     &gcc_options::x_riscv_zc_subext, MASK_ZCF},
  {"zcd",     &gcc_options::x_riscv_zc_subext, MASK_ZCD},

  {"zba", &gcc_options::x_riscv_bitmanip_subext, MASK_ZBA},
  {"zbb", &gcc_options::x_riscv_bitmanip_subext, MASK_ZBB},
  {"zbc", &gcc_options::x_riscv_bitmanip_subext, MASK_ZBC},
  {"zbs", &gcc_options::x_riscv_bitmanip_subext, MASK_ZBS},
  {NULL, NULL, 0}
};

/* Parse a RISC-V ISA string into an option mask.  Must clear or set all arch
   dependent mask bits, in case more than one -march string is passed.  */

static void
riscv_parse_arch_string (const char *isa,
			 struct gcc_options *opts,
			 location_t loc)
{
  riscv_subset_list *subset_list;
  subset_list = riscv_subset_list::parse (isa, loc);
  if (!subset_list)
    return;

  if (opts)
    {
      const riscv_ext_flag_table_t *arch_ext_flag_tab;
      /* Clean up target flags before we set.  */
      for (arch_ext_flag_tab = &riscv_ext_flag_table[0];
	   arch_ext_flag_tab->ext;
	   ++arch_ext_flag_tab)
	opts->*arch_ext_flag_tab->var_ref &= ~arch_ext_flag_tab->mask;

      if (subset_list->xlen () == 32)
	opts->x_target_flags &= ~MASK_64BIT;
      else if (subset_list->xlen () == 64)
	opts->x_target_flags |= MASK_64BIT;

      for (arch_ext_flag_tab = &riscv_ext_flag_table[0];
	   arch_ext_flag_tab->ext;
	   ++arch_ext_flag_tab)
	{
	  if (subset_list->lookup (arch_ext_flag_tab->ext))
	    opts->*arch_ext_flag_tab->var_ref |= arch_ext_flag_tab->mask;
	}
    }

  if (current_subset_list)
    delete current_subset_list;

  current_subset_list = subset_list;
}

/* Return the riscv_cpu_info entry for CPU, NULL if not found.  */

const riscv_cpu_info *
riscv_find_cpu (const char *cpu)
{
  const riscv_cpu_info *cpu_info = &riscv_cpu_tables[0];
  for (;cpu_info->name != NULL; ++cpu_info)
    {
      const char *name = cpu_info->name;
      if (strcmp (cpu, name) == 0)
	return cpu_info;
    }
  return NULL;
}

/* Implement TARGET_HANDLE_OPTION.  */

static bool
riscv_handle_option (struct gcc_options *opts,
		     struct gcc_options *opts_set ATTRIBUTE_UNUSED,
		     const struct cl_decoded_option *decoded,
		     location_t loc)
{
  switch (decoded->opt_index)
    {
    case OPT_march_:
      riscv_parse_arch_string (decoded->arg, opts, loc);
      return true;

    case OPT_mcpu_:
      if (riscv_find_cpu (decoded->arg) == NULL)
	error_at (loc, "%<-mcpu=%s%>: unknown CPU",
		  decoded->arg);
      return true;

    default:
      return true;
    }
}

/* Expand arch string with implied extensions.  */

const char *
riscv_expand_arch (int argc ATTRIBUTE_UNUSED,
		   const char **argv)
{
  gcc_assert (argc == 1);
  location_t loc = UNKNOWN_LOCATION;
  riscv_parse_arch_string (argv[0], NULL, loc);
  const std::string arch = riscv_arch_str (false);
  if (arch.length())
    return xasprintf ("-march=%s", arch.c_str());
  else
    return "";
}

/* Expand default -mtune option from -mcpu option, use default --with-tune value
   if -mcpu don't have valid value.  */

const char *
riscv_default_mtune (int argc, const char **argv)
{
  gcc_assert (argc == 2);
  const riscv_cpu_info *cpu = riscv_find_cpu (argv[0]);
  const char *default_mtune = argv[1];
  if (cpu)
    return cpu->tune;
  else
    return default_mtune;
}

/* Compute default -mabi option from arch string.  */

static const char *
compute_default_abi(const char* arch_str)
{
  location_t loc = UNKNOWN_LOCATION;

  riscv_parse_arch_string (arch_str, NULL, loc);
  const riscv_subset_list * subset_list = riscv_current_subset_list ();
  if (subset_list == NULL)
    return "";

  if (subset_list->xlen() == 64) {
    if (TARGET_SUBSET_VERSION_P("d", RISCV_DONT_CARE_VERSION,
				RISCV_DONT_CARE_VERSION))
      return "-mabi=lp64d";
    if (TARGET_SUBSET_VERSION_P("f", RISCV_DONT_CARE_VERSION,
				RISCV_DONT_CARE_VERSION))
      return "-mabi=lp64f";
    return "-mabi=lp64";
  } else {
    if (TARGET_SUBSET_VERSION_P("e", RISCV_DONT_CARE_VERSION,
				RISCV_DONT_CARE_VERSION))
      return "-mabi=ilp32e";
    if (TARGET_SUBSET_VERSION_P("d", RISCV_DONT_CARE_VERSION,
				RISCV_DONT_CARE_VERSION))
      return "-mabi=ilp32d";
    if (TARGET_SUBSET_VERSION_P("f", RISCV_DONT_CARE_VERSION,
				RISCV_DONT_CARE_VERSION))
      return "-mabi=ilp32f";
    return "-mabi=ilp32";
  }
}

/* Expand default -mabi option from -march option.  */

const char *
riscv_expand_abi_from_arch (int argc ATTRIBUTE_UNUSED,
			    const char **argv ATTRIBUTE_UNUSED)
{
  return compute_default_abi(argv[0]);
}

/* Expand default -mabi option from -mcpu option.  */

const char *
riscv_expand_abi_from_cpu (int argc, const char **argv)
{
  gcc_assert (argc > 0 && argc <= 2);
  const char *default_abi_str = NULL;
  if (argc >= 2)
    default_abi_str = argv[1];

  const riscv_cpu_info *cpu = riscv_find_cpu (argv[0]);

  if (cpu == NULL)
    {
      if (default_abi_str == NULL)
	return "";
      else
	return xasprintf ("-mabi=%s", default_abi_str);
    }
  else
    return compute_default_abi(cpu->arch);
}

/* Expand arch string with implied extensions from -mcpu option.  */

const char *
riscv_expand_arch_from_cpu (int argc ATTRIBUTE_UNUSED,
			    const char **argv)
{
  gcc_assert (argc > 0 && argc <= 2);
  const char *default_arch_str = NULL;
  const char *arch_str = NULL;
  if (argc >= 2)
    default_arch_str = argv[1];

  const riscv_cpu_info *cpu = riscv_find_cpu (argv[0]);

  if (cpu == NULL)
    {
      if (default_arch_str == NULL)
	return "";
      else
	arch_str = default_arch_str;
    }
  else
    arch_str = cpu->arch;

  location_t loc = UNKNOWN_LOCATION;

  riscv_parse_arch_string (arch_str, NULL, loc);
  const std::string arch = riscv_arch_str (false);
  return xasprintf ("-march=%s", arch.c_str());
}

/* Get multilib path for STARTFILE_PREFIX_SPEC.  */

const char *
riscv_get_arch_spec_path (int argc ATTRIBUTE_UNUSED,
	      const char **argv ATTRIBUTE_UNUSED)
{
  const char *arch_spec = "";
  const char *abi_spec = NULL;
  const char *xlen_spec = NULL;

  const riscv_subset_list *subset_list = riscv_current_subset_list ();

  if (subset_list)
    {
      if (subset_list->xlen() == 64)
	xlen_spec = "lib64";
      else if (subset_list->xlen() == 32)
	xlen_spec = "lib32";
      else
	gcc_unreachable ();

      if (subset_list->lookup ("xtheadc"))
	{
	  riscv_subset_t *subset = subset_list->lookup ("v");
	  if (subset)
	    {
	      if (subset->major_version == 0 && subset->minor_version == 7)
		arch_spec = "v0p7_xthead";
	      else if (subset->major_version == 1 && subset->minor_version == 0)
		arch_spec = "v_xthead";
	      else
		fatal_error (input_location, "Only v0p7 and v1p0 are supported");
	    }
	  else
	    arch_spec = "xthead";
	}
    }

  for (int i = 0; i < argc; i++)
    {
      if (strcmp (argv[i], "-") == 0 || argv[i] == NULL)
	continue;

      if (strstr(argv[i], "mabi=") != NULL)
	abi_spec = argv[i] + strlen("mabi=");

      if (!xlen_spec)
	{
	  if (strstr(argv[i], "rv64") != NULL)
	    xlen_spec = "lib64";
	  if (strstr(argv[i], "rv32") != NULL)
	    xlen_spec = "lib32";
	}
    }

  if (!xlen_spec)
    warning_at (input_location, 0, "Missing -march=");

  if (!abi_spec)
    warning_at (input_location, 0, "Missing -mabi=");

  return concat (xlen_spec, arch_spec, "/", abi_spec, "/", NULL);
}

/* Find last switch with the prefix, options are take last one in general,
   return NULL if not found, and return the option value if found, it could
   return empty string if the option has no value.  */

static const char *
find_last_appear_switch (
  const struct switchstr *switches,
  int n_switches,
  const char *prefix)
{
  int i;
  size_t len = strlen (prefix);

  for (i = 0; i < n_switches; ++i)
    {
      const struct switchstr *this_switch = &switches[n_switches - i - 1];

      if (this_switch->live_cond & SWITCH_FALSE)
	continue;

      if (strncmp (this_switch->part1, prefix, len) == 0)
	return this_switch->part1 + len;
    }

  return NULL;
}

/* Utils functions to check STR is start with PREFIX or not.  */

static bool
prefixed_with (const std::string &str, const char *prefix)
{
  return strncmp (prefix, str.c_str (), strlen (prefix)) == 0;
}

/* Parse the path and cond string into riscv_multi_lib_info_t, return false
   if parsing failed. */

bool
riscv_multi_lib_info_t::parse (
  struct riscv_multi_lib_info_t *multi_lib_info,
  const std::string &path,
  const std::vector<std::string> &conds)
{
  const char *default_arch_str = STRINGIZING (TARGET_RISCV_DEFAULT_ARCH);
  const char *default_abi_str = STRINGIZING (TARGET_RISCV_DEFAULT_ABI);
  multi_lib_info->conds = conds;
  if (path == ".")
    {
      multi_lib_info->arch_str = default_arch_str;
      multi_lib_info->abi_str = default_abi_str;
    }
  else
    {
      std::vector<std::string>::const_iterator itr;
      for (itr = conds.begin (); itr != conds.end (); ++itr)
	if (prefixed_with (*itr, "march="))
	  multi_lib_info->arch_str = itr->c_str () + strlen ("march=");
	else if (prefixed_with (*itr, "mabi="))
	  multi_lib_info->abi_str = itr->c_str () + strlen ("mabi=");

	/* Skip this multi-lib if this configuration is exactly same as
	   default multi-lib settings.  */
      if (multi_lib_info->arch_str == default_arch_str
	  && multi_lib_info->abi_str == default_abi_str)
	return false;
    }

  multi_lib_info->subset_list =
    riscv_subset_list::parse (multi_lib_info->arch_str.c_str (), input_location);

  return true;
}

/* Report error if not found suitable multilib.  */
const char *
riscv_multi_lib_check (int argc ATTRIBUTE_UNUSED,
		       const char **argv ATTRIBUTE_UNUSED)
{
  if (riscv_no_matched_multi_lib)
    fatal_error (
      input_location,
      "Can't find suitable multilib set for %<-march=%s%>/%<-mabi=%s%>",
      riscv_current_arch_str.c_str (),
      riscv_current_abi_str.c_str ());

  return "";
}

/* Checking ARG is not appeared in SWITCHES if NOT_ARG is set or
   ARG is appeared if NOT_ARG is not set.  */

static bool
riscv_check_cond (
  const struct switchstr *switches,
  int n_switches,
  const std::string &arg,
  bool not_arg)
{
  int i;
  for (i = 0; i < n_switches; ++i)
    {
      const struct switchstr *this_switch = &switches[n_switches - i - 1];

      if ((this_switch->live_cond & SWITCH_IGNORE) != 0)
	continue;

      if (this_switch->live_cond & SWITCH_FALSE)
	continue;

      /* ARG should not appear if NOT_ARG is set.  */
      if (arg == this_switch->part1)
	return not_arg ? false : true;
    }

  /* Not found ARG? that's ok if NOT_ARG is not set.  */
  return not_arg ? true : false;
}

/* Check the other cond is found or not, return -1 if we should reject this
   multi-lib option set, otherwise return updated MATCH_SCORE.   */

static int
riscv_check_conds (
  const struct switchstr *switches,
  int n_switches,
  int match_score,
  const std::vector<std::string> &conds)
{
  bool not_arg;
  bool ok;
  int ok_count = 0;
  std::vector<std::string>::const_iterator itr;
  const char *checking_arg;

  if (match_score == -1)
    return -1;

  for (itr = conds.begin (); itr != conds.end (); ++itr)
    {
      /* We'll check march= and mabi= in ohter place.  */
      if (prefixed_with (*itr, "march=") || prefixed_with (*itr, "mabi="))
	continue;

      checking_arg = itr->c_str ();
      if (checking_arg[0] == '!')
	{
	  not_arg = true;
	  /* Skip '!'. */
	  checking_arg = checking_arg + 1;
	}
      else
	not_arg = false;

      ok = riscv_check_cond (switches, n_switches, checking_arg, not_arg);

      if (!ok)
	return -1;

      ok_count++;
    }

  /* 100 is magic number, it's just used for make sure this multi-lib has
     higher priority if we found any some option is listed in the option check
     list. */
  return match_score + ok_count * 100;
}

/* We only override this in bare-metal toolchain.  */
#ifdef RISCV_USE_CUSTOMISED_MULTI_LIB

/* Implement TARGET_COMPUTE_MULTILIB.  */
static const char *
riscv_compute_multilib (
  const struct switchstr *switches,
  int n_switches,
  const char *multilib_dir,
  const char *multilib_defaults ATTRIBUTE_UNUSED,
  const char *multilib_select,
  const char *multilib_matches ATTRIBUTE_UNUSED,
  const char *multilib_exclusions ATTRIBUTE_UNUSED,
  const char *multilib_reuse ATTRIBUTE_UNUSED)
{
  const char *p;
  const char *this_path;
  size_t this_path_len;
  bool result;
  riscv_no_matched_multi_lib = false;
  riscv_subset_list *subset_list = NULL;

  std::vector<riscv_multi_lib_info_t> multilib_infos;
  std::vector<std::string> option_conds;
  std::string option_cond;
  riscv_multi_lib_info_t multilib_info;

  /* Find march.  */
  riscv_current_arch_str =
    find_last_appear_switch (switches, n_switches, "march=");
  /* Find mabi.  */
  riscv_current_abi_str =
    find_last_appear_switch (switches, n_switches, "mabi=");

  /* Failed to find -march or -mabi, but it should not happened since we have
     set both in OPTION_DEFAULT_SPECS.  */
  if (riscv_current_arch_str.empty () || riscv_current_abi_str.empty ())
    return multilib_dir;

  subset_list = riscv_subset_list::parse (riscv_current_arch_str.c_str (),
					  input_location);

  /* Failed to parse -march, fallback to using what gcc use.  */
  if (subset_list == NULL)
    return multilib_dir;

  /* Parsing MULTILIB_SELECT, ignore MULTILIB_REUSE here, we have our own rules.
     TODO: most codes are grab from gcc.c, maybe we should refine that?  */
  p = multilib_select;

  while (*p != '\0')
    {
      /* Ignore newlines.  */
      if (*p == '\n')
	{
	  ++p;
	  continue;
	}

      /* Format of each multilib:
	 <path> <opt1> <opt2> ... <optN>;  */
      /* Get the path.  */
      const char *q = NULL;
      this_path = p;
      while (*p != ' ')
	{
	  if (*p == '\0')
	    {
	      fatal_error (input_location, "multilib select %qs %qs is invalid",
			   multilib_select, multilib_reuse);
	    }
	  if (!q && *p == ':')
	    q = p;
	  ++p;
	}

      this_path_len = (!q ? p - this_path : q - this_path);
      multilib_info.path = std::string (this_path, this_path_len);

      option_conds.clear ();
      /* Pasrse option check list into vector<string>.
	 e.g. "march=rv64imafd mabi=lp64 !mcmodel=medany" to
	      ["march=rv64imafd", "mabi=lp64", "!mcmodel=medany"].  */
      while (*p != ';')
	{
	  option_cond = "";
	  /* Skip space.  */
	  while (*p == ' ') p++;

	  while (*p && *p != ' ' && *p != ';')
	      option_cond.push_back (*p++);

	  /* Ignore `!march=` and `!mabi=`, we will handle march and mabi
	     later. */
	  if (option_cond.size ()
	      && !prefixed_with (option_cond, "!march=")
	      && !prefixed_with (option_cond, "!mabi="))
	    option_conds.push_back (option_cond);
	}

      result =
	riscv_multi_lib_info_t::parse (
	  &multilib_info,
	  std::string (this_path, this_path_len),
	  option_conds);

      if (result)
	multilib_infos.push_back (multilib_info);

      p++;
    }

  int match_score = -1;
  int max_match_score = 0;
  int best_match_multi_lib = -1;
  /* Try to decision which set we should used.  */
  /* We have 3 level decision tree here, ABI, check input arch/ABI must
     be superset of multi-lib arch, and other rest option checking.  */
  for (size_t i = 0; i < multilib_infos.size (); ++i)
    {
      /* Check ABI is same first.  */
      if (riscv_current_abi_str != multilib_infos[i].abi_str)
	continue;

      /* Found a potential compatible multi-lib setting!
	 Calculate the match score.  */
      match_score = subset_list->match_score (multilib_infos[i].subset_list);

      /* Checking other cond in the multi-lib setting.  */
      match_score = riscv_check_conds (switches,
				       n_switches,
				       match_score,
				       multilib_infos[i].conds);

      /* Record highest match score multi-lib setting.  */
      if (match_score >= max_match_score)
	{
	  best_match_multi_lib = i;
	  max_match_score = match_score;
	}
    }

  if (best_match_multi_lib == -1)
    {
      riscv_no_matched_multi_lib = true;
      return multilib_dir;
    }
  else
    return xstrdup (multilib_infos[best_match_multi_lib].path.c_str ());
}

#undef TARGET_COMPUTE_MULTILIB
#define TARGET_COMPUTE_MULTILIB riscv_compute_multilib
#endif

/* Implement TARGET_OPTION_OPTIMIZATION_TABLE.  */
static const struct default_options riscv_option_optimization_table[] =
  {
    { OPT_LEVELS_1_PLUS, OPT_fsection_anchors, NULL, 1 },
    { OPT_LEVELS_2_PLUS, OPT_free, NULL, 1 },
    { OPT_LEVELS_NONE, 0, NULL, 0 }
  };

#undef TARGET_OPTION_OPTIMIZATION_TABLE
#define TARGET_OPTION_OPTIMIZATION_TABLE riscv_option_optimization_table

#undef TARGET_HANDLE_OPTION
#define TARGET_HANDLE_OPTION riscv_handle_option

struct gcc_targetm_common targetm_common = TARGETM_COMMON_INITIALIZER;
