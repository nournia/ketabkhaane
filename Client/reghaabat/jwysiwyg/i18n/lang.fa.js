/**
 * Internationalization: Farsi Language
 * 
 * Depends on jWYSIWYG, $.wysiwyg.i18n
 *
 */
(function ($) {
	if (undefined === $.wysiwyg) {
		throw "lang.fa.js depends on $.wysiwyg";
	}
	if (undefined === $.wysiwyg.i18n) {
		throw "lang.fa.js depends on $.wysiwyg.i18n";
	}

	$.wysiwyg.i18n.lang.fa = {
		controls: {
			"Bold": "درشت",
			"Colorpicker": "انتخاب رنگ",
			"Copy": "کپی",
			"Create link": "پیوند",
			"Cut": "برش دادن",
			"Decrease font size": "کاهش اندازه قلم",
			"Fullscreen": "تمام صفحه",
			"Header 1": "سرفصل 1",
			"Header 2": "سرفصل 2",
			"Header 3": "سرفصل 3",
			"View source code": "کد منبع",
			"Increase font size": "افزایش اندازه قلم",
			"Indent": "تو رفتگی",
			"Insert Horizontal Rule": "خط افقی",
			"Insert image": "تصویر",
			"Insert Ordered List": "لیست شماره‌دار",
			"Insert table": "جدول",
			"Insert Unordered List": "لیست بدون شماره",
			"Italic": "خوابیده",
			"Justify Center": "وسط",
			"Justify Full": "کامل",
			"Justify Left": "چپ",
			"Justify Right": "راست",
			"Left to Right": "چپ به راست",
			"Outdent": "بیرون آمدگی",
			"Paste": "بازگردانی",
			"Redo": "تکرار حرکت قبلی",
			"Remove formatting": "حذف قالب",
			"Right to Left": "راست به چپ",
			"Strike-through": "خط روی نوشته",
			"Subscript": "زیرنویس",
			"Superscript": "بالانویش",
			"Underline": "خط زیر متن",
			"Undo": "بازگشتن به آخرین حرکت"
		},

		dialogs: {
			// for all
			"Apply": "تایید",
			"Cancel": "لغو",

			colorpicker: {
				"Colorpicker": "انتخاب رنگ",
				"Color": "رنگ"
			},

			image: {
				"Insert Image": "تصویر",
				"Preview": "پیش نمایش",
				"URL": "آدرس",
				"Title": "عنوان",
				"Description": "توضیح",
				"Width": "پهنا",
				"Height": "ارتفاع",
				"Original W x H": "ابعاد واقعی",
				"Float": "چیدمان",
				"None": "انتخاب نشده",
				"Left": "چپ",
				"Right": "راست"
			},

			link: {
				"Insert Link": "پیوند",
				"Link URL": "آدرس",
				"Link Title": "عنوان",
				"Link Target": ""
			},

			table: {
				"Insert table": "جدول",
				"Count of columns": "ستونها",
				"Count of rows": "ردیفها"
			}
		}
	};
})(jQuery);