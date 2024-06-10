#ifndef LOADER_BASIC_ELF_FILE_H
#define LOADER_BASIC_ELF_FILE_H

#include "../../abstract/utils/udata.h"
#include "../../abstract/utils/raw_file.h"

#include "elf.h"

#include <type_traits>
#include <string>
#include <memory>
#include <iostream>

using namespace std;


template<int CLASS>
struct basic_elf_file;

template<int CLASS>
basic_elf_file<CLASS> parse_elf_from_raw_file(raw_file &&file);

template<int CLASS>
raw_file generate_file_from_custom_elf(basic_elf_file<CLASS> &&elf);


bool check_elf_magic(raw_file &file);

int check_elf_class(raw_file &file);


template<int CLASS>
struct basic_elf_file {
    static_assert(CLASS
                  == 64 || CLASS == 32);

    typedef typename conditional<CLASS == 64, Elf64_Half, Elf32_Half>::type architecture_type;
    typedef typename conditional<CLASS == 64, Elf64_Half, Elf32_Half>::type object_type;
    typedef typename conditional<CLASS == 64, Elf64_Word, Elf32_Word>::type object_version;
    typedef typename conditional<CLASS == 64, Elf64_Addr, Elf32_Addr>::type entry_addr;

    typedef typename conditional<CLASS == 64, Elf64_Ehdr, Elf32_Ehdr>::type header;
    typedef typename conditional<CLASS == 64, Elf64_Phdr, Elf32_Phdr>::type segment;
    typedef typename conditional<CLASS == 64, Elf64_Shdr, Elf32_Shdr>::type section;
    typedef typename conditional<CLASS == 64, Elf64_auxv_t, Elf32_auxv_t>::type auxiliary;
    typedef typename conditional<CLASS == 64, Elf64_Dyn, Elf32_Dyn>::type dyn;
    typedef typename conditional<CLASS == 64, Elf64_Rel, Elf32_Rel>::type rel;
    typedef typename conditional<CLASS == 64, Elf64_Rela, Elf32_Rela>::type rela;
    typedef typename conditional<CLASS == 64, Elf64_Sym, Elf32_Sym>::type sym;

    static auto ELF_R_SYM(auto x) {
        if constexpr (CLASS == 64)
            return ELF64_R_SYM(x);
        else if constexpr (CLASS == 32)
            return ELF32_R_SYM(x);
    }

    static auto ELF_R_TYPE(auto x) {
        if constexpr (CLASS == 64)
            return ELF64_R_TYPE(x);
        else if constexpr (CLASS == 32)
            return ELF32_R_TYPE(x);
    }

    static auto ELF_ST_BIND(auto x) {
        if constexpr (CLASS == 64)
            return ELF64_ST_BIND(x);
        else if constexpr (CLASS == 32)
            return ELF32_ST_BIND(x);
    }

    static auto ELF_ST_TYPE(auto x) {
        if constexpr (CLASS == 64)
            return ELF64_ST_TYPE(x);
        else if constexpr (CLASS == 32)
            return ELF32_ST_TYPE(x);
    }

    typedef typename conditional<CLASS == 64, Elf64_Addr, Elf32_Addr>::type addr;
    typedef typename conditional<CLASS == 64, Elf64_Xword, Elf32_Xword>::type word;
    typedef typename conditional<CLASS == 64, Elf32_Verneed, Elf32_Verneed>::type verneed;
    typedef typename conditional<CLASS == 64, Elf64_Vernaux, Elf32_Vernaux>::type vernaux;
    typedef typename conditional<CLASS == 64, Elf64_Versym, Elf32_Versym>::type versym;


    /** header related */
    header parsed_header;

    uint8_t get_os_type() {
        return parsed_header.e_ident[EI_OSABI];
    }

    object_type get_object_type() {
        return parsed_header.e_type;
    }

    architecture_type get_architecture_type() {
        return parsed_header.e_machine;
    }

    object_version get_version() {
        return parsed_header.e_version;
    }

    entry_addr get_entry_addr() {
        return parsed_header.e_entry;
    }

    // sections info
    inline auto &offset_of_sections_table() {
        return parsed_header.e_shoff;
    }

    inline auto &sz_of_section_table_entry() {
        return parsed_header.e_shentsize;
    }

    inline auto &number_of_sections() {
        return parsed_header.e_shnum;
    }

    // program headers info
    inline auto &offset_of_program_headrs_table() {
        return parsed_header.e_phoff;
    }

    inline auto &sz_of_program_header_table_entry() {
        return parsed_header.e_phentsize;
    }

    inline auto &number_of_program_headers() {
        return parsed_header.e_phnum;
    }


    /** sections related */
    vector<section> parsed_sections;

    int find_section_by_name(const string &name) {
        for (size_t i = 0; i < number_of_sections(); i++) {
            auto section = parsed_sections[i];

            string sec_name = get_string_at_offset_from_strtab((size_t) section.sh_name);
            if (sec_name == name)
                return i;
        }

        return -1;
    }


    /** program headers related */
    vector<segment> parsed_segments;


    /** strings related */
    string get_string_at_offset(size_t off) {
        // building the string
        string str;

        char *ptr = (char *) base_file.offset_in_file(off);

        size_t i = 0;
        while (ptr[i] != '\x00') {
            str += ptr[i];
            i++;
        }

        return str;
    }

    string get_string_at_offset_from_strtab(size_t off) {
        return get_string_at_offset((size_t) parsed_sections[parsed_header.e_shstrndx].sh_offset // the string table section
                                    + off);
    }

    string get_string_at_offset_from_dynstr(size_t off) {
        int in = find_section_by_name(".dynstr");
        return get_string_at_offset((size_t) parsed_sections[in].sh_offset + off);
    }


    /** relocations related */
    rel *get_rel_at_offset(size_t off) {
        return reinterpret_cast<rel *>(base_file.offset_in_file(off));
    }

    rela *get_rela_at_offset(size_t off) {
        return reinterpret_cast<rela *>(base_file.offset_in_file(off));
    }

    /** symbols related */
    sym *get_sym_at_raw_offset(size_t off) {
        return reinterpret_cast<sym *>(base_file.offset_in_file(off));
    }

    sym *find_sym_from_symtab(const string &name) {
        int in = find_section_by_name(".symtab");
        if (in == -1) return nullptr;

        for (size_t i = 0; i < parsed_sections[in].sh_size / parsed_sections[in].sh_entsize; i++) {
            sym *sym = get_sym_at_raw_offset(parsed_sections[in].sh_offset + parsed_sections[in].sh_entsize * i);

            string sym_name = get_string_at_offset_from_dynstr(sym->st_name); // the symbol name

            if (sym_name == name) return sym;
        }

        return nullptr;
    }

    sym *find_sym_from_dynsym(const string &name) {
        int in = find_section_by_name(".dynsym");
        if (in == -1) return nullptr;

        for (size_t i = 0; i < parsed_sections[in].sh_size / parsed_sections[in].sh_entsize; i++) {
            sym *sym = get_sym_at_raw_offset(parsed_sections[in].sh_offset + parsed_sections[in].sh_entsize * i);

            string sym_name = get_string_at_offset_from_dynstr(sym->st_name); // the symbol name

            if (sym_name == name) return sym;
        }

        return nullptr;
    }

    dyn *get_dyn_at_raw_offset(size_t off) {
        return reinterpret_cast<dyn *>(base_file.offset_in_file(off));
    }


    class raw_file base_file;

    /** construct from some base file */
protected:
    basic_elf_file() = delete;

    template<int C>
    friend basic_elf_file<C> parse_elf_from_raw_file(raw_file &&file);

    explicit basic_elf_file(raw_file &&file) : base_file(std::move(file)) {
        this->parsed_header = *reinterpret_cast<header *>(base_file.offset_in_file(0));

        for (int i = 0; i < parsed_header.e_shnum; i++) {
            this->parsed_sections.push_back(*reinterpret_cast<section *>(base_file.offset_in_file(
                    offset_of_sections_table() + sz_of_section_table_entry() * i)));
        }

        for (int i = 0; i < parsed_header.e_phnum; i++) {
            this->parsed_segments.push_back(*reinterpret_cast<segment *>(base_file.offset_in_file(
                    offset_of_program_headrs_table() + sz_of_program_header_table_entry() * i)));
        }
    }

};

template<int CLASS>
basic_elf_file<CLASS> parse_elf_from_raw_file(raw_file &&file) {
    return basic_elf_file<CLASS>(std::move(file));
}

template<int CLASS>
raw_file generate_file_from_custom_elf(basic_elf_file<CLASS> &&elf) {
    raw_file res;
    res.path = elf.base_file.path;

    // copy raw header, will be updated later
    res.content.append((uint8_t *) &elf.parsed_header, sizeof(elf.parsed_header));
#define new_header ((typename basic_elf_file<CLASS>::header *) res.offset_in_file(0))

    vector<
            pair<
                    pair<size_t, size_t>, // start offset, interval sz
                    pair<vector<int>, vector<int>> // section in the interval, segments in this interval
            >
    > master_intervals;
    {
        struct event {
            size_t offset;
            bool start;
            bool section;
            int index;
        };
        vector<event> events;
        for (int i = 0; i < elf.number_of_sections(); i++) {
            cout << "section offset is " << (size_t) elf.parsed_sections[i].sh_offset << endl;
            events.push_back({(size_t) elf.parsed_sections[i].sh_offset, true, true, i});
            events.push_back(
                    {(size_t) elf.parsed_sections[i].sh_offset + (size_t) elf.parsed_sections[i].sh_size, false, true, i});
        }
        for (int i = 0; i < elf.number_of_program_headers(); i++) {
            cout << "segment offset is " << (size_t) elf.parsed_segments[i].p_offset << endl;
            events.push_back({(size_t) elf.parsed_segments[i].p_offset, true, false, i});
            events.push_back(
                    {(size_t) elf.parsed_segments[i].p_offset + (size_t) elf.parsed_segments[i].p_filesz, false, false, i});
        }

        sort(events.begin(), events.end(), [](const event &left, const event &right) -> bool {
            return left.offset < right.offset;
        });


        int curr_active = 0;
        size_t first_offset;
        vector<int> seen_sections;
        vector<int> seen_segments;

        int i = 0;
        while (i < events.size()) {
            size_t curr_offset = events[i].offset;
            cout << "sub interval event offset is " << curr_offset << endl;

            if (curr_active == 0) first_offset = curr_offset;

            while (i < events.size() && events[i].offset == curr_offset) {
                if (events[i].start) {
                    if (events[i].section) seen_sections.push_back(events[i].index);
                    else seen_segments.push_back(events[i].index);

                    curr_active++;
                } else {
                    curr_active--;
                }

                i++;
            }

            if (curr_active == 0) {
                master_intervals.push_back(
                        {{first_offset,  min(curr_offset, elf.base_file.file_size()) - first_offset},
                         {seen_sections, seen_segments}}
                );

                seen_sections.clear();
                seen_segments.clear();
            }
        }

    }
//    for (int i = 0; i < elf.number_of_sections(); i++) {
//        master_intervals.push_back({
//                                           {(size_t) elf.elf_sections[i].sh_offset,
//                                                   (size_t) elf.elf_sections[i].sh_size},
//                                           {{i},   {}}
//                                   });
//    }
//    for (int i = 0; i < elf.number_of_program_headers(); i++) {
//        master_intervals.push_back({
//                                 {(size_t) elf.elf_segments[i].p_offset, (size_t) elf.elf_segments[i].p_filesz},
//                                 {{}, {i}}
//        });
//    }

    cout << "no master intervals " << master_intervals.size() << endl;
    for (int i = 0; i < master_intervals.size(); i++) {
        size_t i_start = master_intervals[i].first.first;
        size_t i_size = master_intervals[i].first.second;

        cout << "curr size is " << res.content.size() << endl;
        cout << "start of interval is " << i_start << endl;
        cout << "size of interval is " << i_size << endl;
        ssize_t offset_diff = (ssize_t) master_intervals[i].first.first - (ssize_t) res.content.size();

        cout << "diff is " << offset_diff << endl;

        res.content.append(
                elf.base_file.offset_in_file(master_intervals[i].first.first),
                master_intervals[i].first.second
        );

        // update the relevant sections and segments
        cout << "updating relevant sections and segments" << endl;
        for (int section: master_intervals[i].second.first)
            elf.parsed_sections[section].sh_offset -= offset_diff;
        for (int segment: master_intervals[i].second.second)
            elf.parsed_segments[segment].p_offset -= offset_diff;
    }

    // write the sections table
    cout << "writing sections " << endl;
    new_header->e_shoff = res.content.size();
    new_header->e_shnum = elf.parsed_sections.size();
    for (int i = 0; i < elf.parsed_sections.size(); i++) {
        res.content.append((uint8_t *) &elf.parsed_sections[i],
                           sizeof(elf.parsed_sections[i]));
        if (elf.get_string_at_offset_from_strtab((size_t) elf.parsed_sections[i].sh_name) == ".shstrtab")
            new_header->e_shstrndx = i;
    }

    // write the segments table
    cout << "writing segments " << endl;
    new_header->e_phoff = res.content.size();
    new_header->e_phnum = elf.parsed_segments.size();
    for (int i = 0; i < elf.parsed_segments.size(); i++)
        res.content.append((uint8_t *) &elf.parsed_segments[i],
                           sizeof(elf.parsed_segments[i]));


    return std::move(res);
}


#endif //LOADER_BASIC_ELF_FILE_H
