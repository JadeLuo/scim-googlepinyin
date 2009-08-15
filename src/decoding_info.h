#include <vector>
#include <string>

#include "pinyinime.h"

#include "pinyin_decoder_service.h"

using std::string;
using std::wstring;

enum ImeState {
    STATE_BYPASS, STATE_IDLE, STATE_INPUT, STATE_COMPOSING, STATE_PREDICT,
    STATE_APP_COMPLETION
};

class DecodingInfo
{
    enum {
        /**
         * Maximum length of the Pinyin string
         */
        PY_STRING_MAX = 28,
        /**
         * Maximum number of candidates to display in one page.
         */
        MAX_PAGE_SIZE_DISPLAY = 10,
    };

    /**
     * Spelling (Pinyin) string.
     */
    std::string m_surface;

    /**
     * The length of surface string successfully decoded by engine.
     */
    int m_surface_decoded_len;
    
    /**
     * Composing string.
     */
    wstring m_composing_str;
    
    /**
     * Length of the active composing string.
     */
    int m_active_cmps_len;
    
    /**
     * Composing string for display, it is copied from m_composing_str, and
     * add spaces between spellings.
     **/
    wstring m_composing_str_display;
    
    /**
     * Length of the active composing string for display.
     */
    int m_active_cmps_display_len;
    
    /**
     * The first full sentence choice.
     */
    wstring m_full_sent;
    
    /**
     * Number of characters which have been fixed.
     */
    int m_fixed_len;

    /**
     * If this flag is true, selection is finished.
     */
    bool m_finish_selection;
    
    /**
     * The starting position for each spelling. The first one is the number
     * of the real starting position elements.
     */
    std::vector<int> m_spl_start;

    /**
     * wrapper for Pinyin-to-Hanzi decoding engine
     */
    PinyinDecoderService m_pinyin_decode_service;
    
    /**
     * Editing cursor in mSurface.
     */
    int m_cursor_pos;
        
    /**
     * The total number of choices for display. The list may only contains
     * the first part. If user tries to navigate to next page which is not
     * in the result list, we need to get these items.
     **/
    int m_total_choices_num;

    ///////////////////////////////////////////////////////////////
    // lookup table stuff
    /**
     * Candidate list. The first one is the full-sentence candidate.
     */
    std::vector<wstring> m_candidates_list;

    /**
     * Element i stores the starting position of page i.
     */
    std::vector<int> m_page_start;

    /**
     * Element i stores the number of characters to page i.
     */
    std::vector<int> m_cn_to_page;
    
    /**
     * The position to delete in Pinyin string. If it is less than 0, IME
     * will do an incremental search, otherwise IME will do a deletion
     * operation. if {@link #m_is_pos_in_spl} is true, IME will delete the whole
     * string for mPosDelSpl-th spelling, otherwise it will only delete
     * m_pos_del_spl-th character in the Pinyin string.
     */
    int m_pos_del_spl;

    /**
     * If {@link #m_pos_del_spl} is big than or equal to 0, this member is used
     * to indicate whether the postion is counted in spelling id or character.
     */
    bool m_is_pos_in_spl;;
    
    ImeState m_ime_state;
    
public:
    DecodingInfo();
    DecodingInfo(const DecodingInfo&);
    void reset();

    bool is_spl_str_full() const;
    
    /**
     * add spelling char
     */
    void add_spl_char(char ch, bool reset);
    
    /**
     * Prepare to delete before cursor. We may delete a spelling char if
     * the cursor is in the range of unfixed part, delete a whole spelling
     * if the cursor in inside the range of the fixed part.
     * This function only marks the position used to delete.
     */
    void prepare_delete_before_cursor();
    
    string get_original_spl_str() const;
    
    int get_spl_str_decoded_len() const;

    wstring get_composing_str() const;
    wstring get_composing_str_active_part() const;
    wstring get_composing_str_for_display() const;
    int get_active_cmps_display_len() const;
    
    wstring get_full_sent() const;
    wstring get_current_full_sent(int active_cand_pos) const;

    void reset_candidates();

    bool can_do_prediction() const;
    bool selection_finished() const;

    wstring get_candidate(int candId) const;
    void get_candidates_for_cache();
    
    bool prepare_page(int page_no);

    size_t get_candidates_number() const;

private:
    // After the user chooses a candidate, input method will do a
    // re-decoding and give the new candidate list.
    // If candidate id is less than 0, means user is inputting Pinyin,
    // not selecting any choice.
    void choose_candidate(int index);
    void update_for_search(int n_candidates);
    void choose_predict_choice(int choice);
    /**
     * the length of m_surface
     */
    int length() const;
};
