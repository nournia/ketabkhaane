/**
 * Controls: Image plugin
 *
 * Depends on jWYSIWYG
 */
(function ($) {
	"use strict";

	if (undefined === $.wysiwyg) {
		throw "wysiwyg.image.js depends on $.wysiwyg";
	}

	if (!$.wysiwyg.controls) {
		$.wysiwyg.controls = {};
	}

	/*
	 * Wysiwyg namespace: public properties and methods
	 */
	$.wysiwyg.controls.image = {
		groupIndex: 6,
		visible: true,
		exec: function () {
			$.wysiwyg.controls.image.init(this);
		},
		tags: ["img"],
		tooltip: "Insert image",	
		init: function (Wysiwyg) {
			var self = this, elements, dialog, formTextLegend, formImageHtml, dialogReplacements, key, translation,
				img = {
					alt: "",
					self: Wysiwyg.dom ? Wysiwyg.dom.getElement("img") : null, // link to element node
					src: "",
					title: ""
				};

			dialogReplacements = {
				legend	: "Insert Image",
				preview : "Preview",
				url     : "URL",
				title   : "Title",
				description : "Description",
				width   : "Width",
				height  : "Height",
				original : "Original W x H",
				"float"	: "Float",
				floatNone : "None",
				floatLeft : "Left",
				floatRight : "Right",
				submit  : "Apply",
				reset   : "Cancel",
				fileManagerIcon : "Select file from server"
			};

			formImageHtml = 		'<form class="wysiwyg" id="wysiwyg-addImage"><fieldset>' +
									'<div class="form-row"><span class="form-row-key">{preview}:</span><div class="form-row-value"><img src="" style="margin: 2px; padding:5px; max-width: 100%; overflow:hidden; max-height: 100px; border: 1px solid rgb(192, 192, 192);"/></div></div>'+
									'<input type="hidden" name="src" value=""/>'+
									// '<div class="form-row"><label for="name">{title}:</label><div class="form-row-value"><input type="text" name="imgtitle" value=""/></div></div>' +
									// '<div class="form-row"><label for="name">{description}:</label><div class="form-row-value"><input type="text" name="description" value=""/></div></div>' +
									'<div class="form-row"><label for="name">{width} × {height}:</label><div class="form-row-value"><input type="text" name="width" value="" class="width-small"/> x <input type="text" name="height" value="" class="width-small"/></div></div>' +
									// '<div class="form-row"><label for="name">{original}:</label><div class="form-row-value"><input type="text" name="naturalWidth" value="" class="width-small" disabled="disabled"/> x ' + '<input type="text" name="naturalHeight" value="" class="width-small" disabled="disabled"/></div></div>' +
									// '<div class="form-row"><label for="name">{float}:</label><div class="form-row-value"><select name="float">' + '<option value="">{floatNone}</option>' + '<option value="left">{floatLeft}</option>' + '<option value="right">{floatRight}</option></select></div></div>' +
									'<div class="form-row form-row-last"><label for="name"></label><div class="form-row-value"><button class="button" id="browseImage">تغییر</button><input type="submit" class="button" value="{submit}"/><input type="reset" class="button" value="{reset}"/></div></div>' +
									'</fieldset></form>';

			for (key in dialogReplacements) {
				if ($.wysiwyg.i18n) {
					translation = $.wysiwyg.i18n.t(dialogReplacements[key], "dialogs.image");

					if (translation === dialogReplacements[key]) { // if not translated search in dialogs 
						translation = $.wysiwyg.i18n.t(dialogReplacements[key], "dialogs");
					}

					dialogReplacements[key] = translation;
				}

				formImageHtml = formImageHtml.replace("{" + key + "}", dialogReplacements[key]);
			}
			formTextLegend = dialogReplacements.legend;

			if (img.self) {
				img.src = img.self.src ? img.self.src : "";
				// img.alt = img.self.alt ? img.self.alt : "";
				// img.title = img.self.title ? img.self.title : "";
				img.width = img.self.width ? img.self.width : "";
				img.height = img.self.height ? img.self.height : "";
			}
			
			var adialog = new $.wysiwyg.dialog(Wysiwyg, {
				"title": formTextLegend,
				"content": formImageHtml
			});
			
			$(adialog).bind("afterOpen", function (e, dialog) {
				$("form#wysiwyg-addImage", dialog).submit(function (e) {
					e.preventDefault();
					self.processInsert(dialog.container, Wysiwyg, img);
					
					adialog.close();
					return false;
				});
				
				// File Manager (select file):
				$("div.wysiwyg-fileManager").bind("click", function () {
					$.wysiwyg.fileManager.init(function (selected) {
						dialog.find("input[name=src]").val(selected);
						dialog.find("input[name=src]").trigger("change");
					});
				});
				
				$("input:reset", dialog).click(function (e) {
					adialog.close();
					
					return false;
				});
				
				$("fieldset", dialog).click(function (e) {
					e.stopPropagation();
				});
				
				// reghaabat section
				
				$("#browseImage").click(function(){
					var fileaddr = containerForm.getFilename();
					
					if (fileaddr == '')
						$("input[type=submit]").click();
					
					dialog.find("input[name=src]").val(fileaddr).change();
					dialog.show();
					return false;
				});
				
				$("input[name=width]").change(function(){
					var w = $("input[name=width]").val(), h = $("input[name=height]").val();
					var r = $('.form-row-value img')[0].naturalWidth / $('.form-row-value img')[0].naturalHeight;
					if (isNaN(w)) w = 0; if (isNaN(h)) h = 0; if (isNaN(r)) r = 1;
					 
					$("input[name=height]").val(Math.round(w * 1/r));					
				});

				$("input[name=height]").change(function(){
					var w = $("input[name=width]").val(), h = $("input[name=height]").val();
					var r = $('.form-row-value img')[0].naturalWidth / $('.form-row-value img')[0].naturalHeight;
					if (isNaN(w)) w = 0; if (isNaN(h)) h = 0; if (isNaN(r)) r = 1;
					 
					$("input[name=width]").val(Math.round(h * r));					
				});
				
				// reghaabat end
				
				self.makeForm(dialog, img);

				if (dialog.find("input[name=src]").val() == "")
				{
					dialog.hide();
					$("#browseImage").click();
				}
			});
			
			adialog.open();
			
			$(Wysiwyg.editorDoc).trigger("editorRefresh.wysiwyg");
		},

		processInsert: function (context, Wysiwyg, img) {
			var image,
				url = $('input[name="src"]', context).val(),
				// title = $('input[name="imgtitle"]', context).val(),
				// description = $('input[name="description"]', context).val(),
				width = $('input[name="width"]', context).val(),
				height = $('input[name="height"]', context).val(),
				// styleFloat = $('select[name="float"]', context).val(),
				style = [],
				found,
				baseUrl;

			if (Wysiwyg.options.controlImage && Wysiwyg.options.controlImage.forceRelativeUrls) {
				baseUrl = window.location.protocol + "//" + window.location.hostname;
				if (0 === url.indexOf(baseUrl)) {
					url = url.substr(baseUrl.length);
				}
			}

			if (img.self) {
				// to preserve all img attributes
				$(img.self).attr("src", url)
					// .attr("title", title)
					// .attr("alt", description)
					// .css("float", styleFloat)
					;

				if (width.toString().match(/^[0-9]+(px|%)?$/)) {
					$(img.self).css("width", width);
				} else {
					$(img.self).css("width", "");
				}

				if (height.toString().match(/^[0-9]+(px|%)?$/)) {
					$(img.self).css("height", height);
				} else {
					$(img.self).css("height", "");
				}
				
				Wysiwyg.saveContent();
			} else {
				found = width.toString().match(/^[0-9]+(px|%)?$/);
				if (found) {
					if (found[1]) {
						style.push("width: " + width + ";");
					} else {
						style.push("width: " + width + "px;");
					}
				}

				found = height.toString().match(/^[0-9]+(px|%)?$/);
				if (found) {
					if (found[1]) {
						style.push("height: " + height + ";");
					} else {
						style.push("height: " + height + "px;");
					}
				}

				// if (styleFloat.length > 0) {
					// style.push("float: " + styleFloat + ";");
				// }

				if (style.length > 0) {
					style = ' style="' + style.join(" ") + '"';
				}

				// if (title && title.length > 0) title = ' title="' + title + '"';
				// if (description && description.length > 0) description = ' alt="' + description + '"';

				image = '<img src="' + url + '"' + /*title + description + */ style + "/>";
				Wysiwyg.insertHtml(image);
			}
		},

		makeForm: function (form, img) {
			form.find("input[name=src]").val(img.src);
			form.find("input[name=imgtitle]").val(img.title);
			form.find("input[name=description]").val(img.alt);
			form.find('input[name="width"]').val(img.width);
			form.find('input[name="height"]').val(img.height);
			form.find('img').attr("src", img.src);

			// reghaabat
			form.find('img').bind("load", function () {
				var iimg = $('.form-row-value img')[0]; 
				if (iimg.naturalWidth && form.find('input[name="width"]').val() == '') {
					form.find('input[name="width"]').val(iimg.naturalWidth);
					form.find('input[name="height"]').val(iimg.naturalHeight);
				}
			});
			

			form.find("input[name=src]").bind("change", function () {
				form.find('img').attr("src", this.value);
			});

			return form;
		}
	};

	$.wysiwyg.insertImage = function (object, url, attributes) {
		return object.each(function () {
			var Wysiwyg = $(this).data("wysiwyg"),
				image,
				attribute;

			if (!Wysiwyg) {
				return this;
			}

			if (!url || url.length === 0) {
				return this;
			}

			if ($.browser.msie) {
				Wysiwyg.ui.focus();
			}

			if (attributes) {
				Wysiwyg.editorDoc.execCommand("insertImage", false, "#jwysiwyg#");
				image = Wysiwyg.getElementByAttributeValue("img", "src", "#jwysiwyg#");

				if (image) {
					image.src = url;

					for (attribute in attributes) {
						if (attributes.hasOwnProperty(attribute)) {
							image.setAttribute(attribute, attributes[attribute]);
						}
					}
				}
			} else {
				Wysiwyg.editorDoc.execCommand("insertImage", false, url);
			}

			Wysiwyg.saveContent();

			$(Wysiwyg.editorDoc).trigger("editorRefresh.wysiwyg");

			return this;
		});
	};
})(jQuery);
