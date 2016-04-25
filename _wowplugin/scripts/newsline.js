
var NewslineEntryType = {
	NEWSLINE_ENTRY_UNKNOWN:		0,
	NEWSLINE_ENTRY_TEXT:		1,
	NEWSLINE_ENTRY_IMAGE:		2,
	NEWSLINE_ENTRY_YOUTUBE:		3
};

//-----------------------------------------------------------------------------------------------------
// Newsline
//-----------------------------------------------------------------------------------------------------
var Newsline = function () {
	var newslineContainer = jQuery('#wowNewslineContainer');

	// Header
	var newslineHeader = jQuery('<div></div>');
	newslineHeader.addClass('wowplugin-page-header not-selectable');
	newslineHeader.appendTo(newslineContainer);

	var newslineHeaderLabel = jQuery('<span>Achievements</span>');
	newslineHeaderLabel.addClass('wowplugin-page-header-label not-selectable');
	newslineHeaderLabel.appendTo(newslineHeader);

	var newslineHeaderToolbar = jQuery('<div></div>');
	newslineHeaderToolbar.addClass('wowplugin-page-header-toolbar not-selectable');
	newslineHeaderToolbar.appendTo(newslineHeader);

	var paginationButtonBack = jQuery('<button id="wowNewslinePaginationBack"></button>');
	paginationButtonBack.addClass('wowplugin-toolbar-button not-selectable');
	paginationButtonBack.appendTo(newslineHeaderToolbar);

	var paginationButtonIconBack = jQuery('<div></div>');
	paginationButtonIconBack.addClass('wowplugin-toolbar-button-icon newsline-pagination-button-icon-back not-selectable');
	paginationButtonIconBack.appendTo(paginationButtonBack);

	var paginationLabel = jQuery('<span id="wowNewslinePaginationLabel"></span>');
	paginationLabel.addClass('newsline-pagination-label not-selectable');
	paginationLabel.appendTo(newslineHeaderToolbar);

	var paginationButtonForward = jQuery('<button id="wowNewslinePaginationForward"></button>');
	paginationButtonForward.addClass('wowplugin-toolbar-button not-selectable');
	paginationButtonForward.appendTo(newslineHeaderToolbar);

	var paginationButtonIconForward = jQuery('<div></div>');
	paginationButtonIconForward.addClass('wowplugin-toolbar-button-icon newsline-pagination-button-icon-forward not-selectable');
	paginationButtonIconForward.appendTo(paginationButtonForward);

	// Content
	var newslineContent = jQuery('<div id="wowNewslineContent"></div>');
	newslineContent.addClass('newsline-content not-selectable');
	newslineContent.appendTo(newslineContainer);

	// Pagination
	this.paginationController = new PaginationController(
		'#wowNewslinePaginationLabel',
		'#wowNewslinePaginationBack',
		'#wowNewslinePaginationForward',
		this.reloadNewslineContent.bind(this)
	);

	this.paginationController.renderPage();
};

Newsline.prototype = {
	//-------------------------------------------------------------------------------------------------
	buildNewslineContent: function (decodedResponse) {
		var container = jQuery('#wowNewslineContent');
		container.empty();

		for (var i = 0; i != decodedResponse.entries.length; ++i) {
			var entryContainer = jQuery('<div></div>');
			entryContainer.addClass('newsline-entry-container not-selectable');
			entryContainer.appendTo(container);

			var entryHeader = jQuery('<div></div>');
			entryHeader.addClass('newsline-entry-header not-selectable');
			entryHeader.appendTo(entryContainer);

			var entryHeaderLabel = jQuery('<span>' + decodedResponse.entries[i].title + '</span>');
			entryHeaderLabel.addClass('newsline-entry-header-label not-selectable');
			entryHeaderLabel.appendTo(entryHeader);

			var entryBody = jQuery('<div></div>');
			entryBody.addClass('newsline-entry-body not-selectable');
			entryBody.appendTo(entryContainer);

			switch (decodedResponse.entries[i].type)
			{
				case NewslineEntryType.NEWSLINE_ENTRY_YOUTUBE:
					var entryContent = jQuery('<div><iframe width="640" height="360" src="https://www.youtube.com/embed/' + decodedResponse.entries[i].contentList[0] + '" frameborder="0" allowfullscreen></iframe></div>');
					entryContent.addClass('newsline-entry-content not-selectable');
					entryContent.appendTo(entryBody);
					break;
			}
		}
	},

	//-------------------------------------------------------------------------------------------------
	queryNewslineError: function (xhr, status, error) {
		console.log(error);
	},

	//-------------------------------------------------------------------------------------------------
	queryNewslineSuccess: function (response, status, xhr) {
		var decodedResponse = null;

		try {
			decodedResponse = JSON.parse(response);
		}
		catch (exception) {
		}

		if (!decodedResponse) {
		}
		else if ('errorMessage' in decodedResponse) {
		}
		else {
			this.buildNewslineContent(decodedResponse);
			this.paginationController.setTotalEntriesQuantity(decodedResponse.totalEntriesQuantity);
		}
	},

	//-------------------------------------------------------------------------------------------------
	reloadNewslineContent: function (skip, limit) {
		var postData = {
			'skip': skip,
			'limit': limit
		};

		jQuery.ajax({
			url: '/_wowplugin/query_newsline.php',
			type: 'POST',
			data: JSON.stringify(postData),
			dataType: 'text',
			error: this.queryNewslineError.bind(this),
			success: this.queryNewslineSuccess.bind(this)
		});
	}
};

//-----------------------------------------------------------------------------------------------------
// Entry point
//-----------------------------------------------------------------------------------------------------
$(document).ready(function () {
	// Styles
	jQuery('<link/>', {
		rel: 'stylesheet',
		type: 'text/css',
		href: '/_wowplugin/styles/common.css'
	}).appendTo('head');

	jQuery('<link/>', {
		rel: 'stylesheet',
		type: 'text/css',
		href: '/_wowplugin/styles/newsline.css'
	}).appendTo('head');

	// Run the building script
	var newsline = new Newsline();
});
