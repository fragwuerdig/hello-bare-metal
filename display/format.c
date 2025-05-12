#include <stdint.h>
#include <stdarg.h>

typedef enum {
    VGA_FORMAT_SPECIFIER_SINT = 0,
    VGA_FORMAT_SPECIFIER_UINT,
    VGA_FORMAT_SPECIFIER_HEX,
    VGA_FORMAT_SPECIFIER_HEX_UPPER,
    VGA_FORMAT_SPECIFIER_OCTAL,
    VGA_FORMAT_SPECIFIER_CHAR,
    VGA_FORMAT_SPECIFIER_STRING,
    VGA_FORMAT_SPECIFIER_POINTER,
    VGA_FORMAT_SPECIFIER_PERCENT,
} vga_format_specifier_t;

typedef enum {
    VGA_FORMAT_FLAG_LEFT_BOUND = 0x01,
    VGA_FORMAT_FLAG_FORCE_SIGN = 0x02,
    VGA_FORMAT_FLAG_SPADDING = 0x04,
    VGA_FORMAT_FLAG_ZPADDING = 0x08,
} vga_format_flag_t;

typedef struct {
    uint8_t specifier;
    uint8_t flags;
} vga_format_spec_t;

uint8_t vga_format_is_flag(char c) {
    return (c == '-' || c == '+');
}

uint64_t vga_format_parse_flags(vga_format_spec_t *spec, const char *format, int pos) {
    int advance = 0;
    while (vga_format_is_flag(format[pos + advance])) {
        if (format[pos + advance] == '-') {
            if (spec->flags & VGA_FORMAT_FLAG_LEFT_BOUND) {
                return 0; // double flag invalid
            }
            spec->flags |= VGA_FORMAT_FLAG_LEFT_BOUND;
            advance++;
            continue;
        } else if (format[pos + advance] == '+') {
            if (spec->flags & VGA_FORMAT_FLAG_FORCE_SIGN) {
                return 0; // double flag invalid
            }
            spec->flags |= VGA_FORMAT_FLAG_FORCE_SIGN;
            advance++;
            continue;
        } else if (format[pos + advance] == ' ') {
            if (spec->flags & VGA_FORMAT_FLAG_SPADDING) {
                return 0; // double flag invalid
            }
            spec->flags |= VGA_FORMAT_FLAG_SPADDING;
            advance++;
            continue;
        } else if (format[pos + advance] == '0') {
            if (spec->flags & VGA_FORMAT_FLAG_ZPADDING) {
                return 0; // double flag invalid
            }
            spec->flags |= VGA_FORMAT_FLAG_ZPADDING;
            advance++;
            continue;
        } else {
            break; // no more flags
        }
    }
    return advance;
}

uint64_t vga_format_parse_specifier(vga_format_spec_t *spec, const char *format, int pos) {
    if (format[pos] == 'd' || format[pos] == 'i') {
        spec->specifier = VGA_FORMAT_SPECIFIER_SINT;
        return 1;
    } else if (format[pos] == 'u') {
        spec->specifier = VGA_FORMAT_SPECIFIER_UINT;
        return 1;
    } else if (format[pos] == 'x') {
        spec->specifier = VGA_FORMAT_SPECIFIER_HEX;
        return 1;
    } else if (format[pos] == 'X') {
        spec->specifier = VGA_FORMAT_SPECIFIER_HEX_UPPER;
        return 1;
    } else {
        return 0; // Unknown specifier
    }
}

uint64_t vga_format_parse(vga_format_spec_t *spec, const char *format) {
    int pos = 0;
    int advance = 0;
    while (format[pos] != '\0') {
        // mandatory % sign
        if (format[pos] != '%') {
            return 0;
        }
        pos++;

        // optional flags
        advance = 0;
        advance = vga_format_parse_flags(spec, format, pos);
        if (advance) {
            pos += advance;
        }

        // mandatory specifier
        advance = 0;
        advance = vga_format_parse_specifier(spec, format, pos);
        if (advance) {
            pos += advance;
            return pos;
        } else {
            return 0;
        }
    }
    return 0; // string ends early
}

uint64_t vga_write_hex(uint64_t value, char * buffer, uint64_t size, uint8_t capitalize) {
 
    int64_t siz_track = size;
    char * ptr = buffer;
    char reverse[32];

    if (siz_track <= 0) {
        return 0; // no space to write
    }

    if (siz_track > 32) {
        siz_track = 32; // max size
    }

    // write the digits in reverse order
    do {
        uint8_t digit = value % 16;
        value /= 16;
        if (capitalize) {
            digit = digit < 10 ? digit + '0' : digit - 10 + 'A';
        } else {
            digit = digit < 10 ? digit + '0' : digit - 10 + 'a';
        }
        *ptr = digit;
        ptr++;
        siz_track--;
        if (siz_track <= 0) {
            return 0; // no space to write
        }    
    } while (value >= 1);

    uint64_t num_digits = ptr-buffer;
    for (uint64_t i = 0; i < num_digits; i++) {
        reverse[i] = buffer[num_digits - i - 1];
    }
    for (uint64_t i = 0; i < num_digits; i++) {
        buffer[i+2] = reverse[i];
    }
    buffer[0] = '0';
    buffer[1] = 'x';

    return num_digits+2;

}

uint64_t vga_write_dec(uint64_t value, char * buffer, uint64_t size) {

    uint64_t siz_track = size;
    char * ptr = buffer;
    char reverse[32];

    if (siz_track <= 0) {
        return 0; // no space to write
    }

    if (siz_track > 32) {
        siz_track = 32; // max size
    }

    // write the digits in reverse order
    do {
        uint8_t digit = value % 10;
        value /= 10;
        char c = '0' + digit;
        *ptr = c;
        ptr++;
        siz_track--;
        if (siz_track <= 0) {
            return 0; // no space to write
        }    
    } while (value >= 1);

    // correct the order
    uint64_t num_digits = ptr - buffer;
    for (uint64_t i = 0; i < num_digits; i++) {
        reverse[i] = buffer[num_digits - i - 1];
        
    }
    for (uint64_t i = 0; i < num_digits; i++) {
        buffer[i] = reverse[i];
    }

    return num_digits;

}

uint64_t vga_format_print_uint(uint64_t value, vga_format_spec_t *, void * out, uint64_t out_size) {

    if (out_size <= 0) {
        return 0; // no space to write
    }

    char * ptr = (char *)out;
    uint64_t siz_track = out_size;

    if (siz_track <= 0) {
        return 0; // no space to write
    }

    uint8_t digits = vga_write_dec(value, ptr, siz_track);
    ptr += digits;
    siz_track -= digits;

    return ptr - (char *)out;

}

uint64_t vga_format_print_hex(uint64_t value, vga_format_spec_t *spec, void * out, uint64_t out_size) {

    if (out_size <= 0) {
        return 0; // no space to write
    }

    char * ptr = (char *)out;
    uint64_t siz_track = out_size;

    if (siz_track <= 0) {
        return 0; // no space to write
    }

    uint8_t digits = vga_write_hex(value, ptr, siz_track, spec->specifier == VGA_FORMAT_SPECIFIER_HEX_UPPER);
    ptr += digits;
    siz_track -= digits;

    return ptr - (char *)out;

}

uint64_t vga_format_print_sint(int64_t value, vga_format_spec_t *, void * out, uint64_t out_size) {

    if (out_size <= 0) {
        return 0; // no space to write
    }

    char * ptr = (char *)out;
    uint64_t siz_track = out_size;

    if (siz_track <= 0) {
        return 0; // no space to write
    }

    if (value < 0) {
        ptr[0] = '-';
        ptr++;
        siz_track--;
    }

    if (siz_track <= 0) {
        return 0; // no space to write
    }

    uint64_t abs_value = value < 0 ? -value : value;
    uint8_t digits = vga_write_dec(abs_value, ptr, siz_track);
    ptr += digits;
    siz_track -= digits;

    return ptr - (char *)out;

}

uint64_t vga_snprintf(char * out, uint64_t size, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int advance = 0;
    uint64_t siz_track = size;
    char * ptr = out;
    char * format_ptr = format;

    if ( siz_track <= 0 ) {
        return 0; // no space to write
    }

    while (format_ptr[0] != '\0') {
        if (siz_track <= 0) {
            break; // no space to write
        }
        vga_format_spec_t spec = {0};
        advance = vga_format_parse(&spec, format_ptr);
        if (advance == 0) {
            ptr[0] = format_ptr[0];
            format_ptr++;
            ptr++;
            siz_track--;
            continue;
        }
        uint64_t digits;
        int64_t value_int = 0;
        uint64_t value_uint = 0;
        uint64_t value_hex = 0;
        switch (spec.specifier) {
            case VGA_FORMAT_SPECIFIER_SINT:
                value_int = va_arg(args, int64_t);
                digits = vga_format_print_sint(value_int, &spec, ptr, siz_track);
                break;
            case VGA_FORMAT_SPECIFIER_UINT:
            value_uint = va_arg(args, uint64_t);
                digits = vga_format_print_uint(value_uint, &spec, ptr, siz_track);
                break;
            case VGA_FORMAT_SPECIFIER_HEX:
            case VGA_FORMAT_SPECIFIER_HEX_UPPER:
                value_hex = va_arg(args, uint64_t);
                digits = vga_format_print_hex(value_hex, &spec, ptr, siz_track);
                break;
            default:
                *ptr = *format;
                ptr++;
                siz_track--;
                continue;
        }
        format_ptr += advance;
        ptr += digits;
        siz_track -= digits;
    }

    ptr[0] = '\0'; // null terminate the string

    va_end(args);
    return ptr - out; // return the number of characters written
}