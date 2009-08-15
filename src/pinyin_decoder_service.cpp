#include <cassert>
#include <algorithm>
#include <cstring>

#include "pinyinime.h"
#include "userdict.h"

#include "pinyin_decoder_service.h"

using namespace ime_pinyin;

static const int RET_BUF_LEN = 256;
static char16 retbuf[RET_BUF_LEN];
static char16 (*predict_buf)[kMaxPredictSize + 1] = NULL;
static size_t predict_len;

wstring char16_to_wstr(char16* s, size_t len)
{
    wstring str;
    copy(s, s+len, std::back_inserter(str));
    return str;
}

size_t
PinyinDecoderService::search(const string& spelling_str)
{
    return im_search(spelling_str.c_str(), spelling_str.length());
}

void
PinyinDecoderService::reset_search()
{
    im_reset_search();
}

int
PinyinDecoderService::choose(int choice)
{
    return im_choose(choice);
}

vector<int>
PinyinDecoderService::get_spelling_start()
{
    const unsigned short *spelling_start;
    size_t len = im_get_spl_start_pos(spelling_start);
    return vector<int> (spelling_start, spelling_start + len);
}

wstring
PinyinDecoderService::get_predict_item(int predict_no)
{
    wstring str;
    if (predict_no < 0 || (size_t)predict_no >= predict_len) {
        str = char16_to_wstr(predict_buf[0], 0);
    } else {
        str = char16_to_wstr(predict_buf[predict_no],
                             utf16_strlen(predict_buf[predict_no]));
    }
}


list<wstring>
PinyinDecoderService::get_predict_list(int predicts_start, int predicts_num)
{
    list<wstring> predict_list;
    for (int i = predicts_start; i < predicts_start + predicts_num; i++) {
        predict_list.push_back(get_predict_item(i));
    }
    return predict_list;
}

wstring
PinyinDecoderService::get_choice(int index)
{
    if (im_get_candidate(index, retbuf, RET_BUF_LEN)) {
        return char16_to_wstr(retbuf, utf16_strlen(retbuf));
    } else {
        return char16_to_wstr(retbuf, 0);
    }
}


list<wstring>
PinyinDecoderService::get_choice_list(int choices_start, int choices_num,
                                      int sent_fixed_len)
{
    list<wstring> choice_list;
    for (int i = choices_start; i < choices_start + choices_num; i++) {
        choice_list.push_back(get_choice(i));
    }
    return choice_list;
}

size_t
PinyinDecoderService::get_fixed_len()
{
    return im_get_fixed_len();
}

string
PinyinDecoderService::get_py_str(bool decoded)
{
    size_t py_len;
    const char *py = im_get_sps_str(&py_len);
    assert(py != NULL);
    if (!decoded)
        py_len = strlen(py);
    
    const unsigned short *spl_start;
    size_t len;
    len = im_get_spl_start_pos(spl_start);

    return string(py, py_len);
}