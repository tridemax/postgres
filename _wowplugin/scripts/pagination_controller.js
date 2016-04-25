
//-----------------------------------------------------------------------------------------------------
// PaginationController
//-----------------------------------------------------------------------------------------------------
var PaginationController = function (domLabel, domBack, domForward, renderer) {
	this.domLabel = domLabel;
	this.domBack = domBack;
	this.domForward = domForward;
	this.renderer = renderer;

	this.currentSkip = 0;
	this.currentLimit = 4;
	this.totalEntriesQuantity = 0;

	jQuery(this.domBack).on('click', this.switchPageBack.bind(this));
	jQuery(this.domForward).on('click', this.switchPageForward.bind(this));
};

PaginationController.prototype = {
	//-------------------------------------------------------------------------------------------------
	updateLabel: function () {
		var from = this.totalEntriesQuantity == 0 ? 0 : this.currentSkip + 1;
		var to = Math.min(this.currentSkip + this.currentLimit, this.totalEntriesQuantity);
		var total = this.totalEntriesQuantity;

		jQuery(this.domLabel).html(from.toString() + '-' + to.toString() + ' of ' + total.toString());
	},

	//-------------------------------------------------------------------------------------------------
	setTotalEntriesQuantity: function (totalEntriesQuantity) {
		this.totalEntriesQuantity = totalEntriesQuantity;
		this.updateLabel();

		jQuery(this.domBack).prop('disabled', this.currentSkip == 0);
		jQuery(this.domForward).prop('disabled', this.currentSkip + this.currentLimit >= this.totalEntriesQuantity);
	},

	//-------------------------------------------------------------------------------------------------
	renderPage: function () {
		this.renderer(this.currentSkip, this.currentLimit);
	},

	//-------------------------------------------------------------------------------------------------
	switchPageBack: function () {
		if (this.currentSkip != 0) {
			this.currentSkip = Math.max(0, this.currentSkip - this.currentLimit);

			this.renderer(this.currentSkip, this.currentLimit);
		}
	},

	//-------------------------------------------------------------------------------------------------
	switchPageForward: function () {
		if (this.currentSkip + this.currentLimit < this.totalEntriesQuantity) {
			this.currentSkip += this.currentLimit;

			this.renderer(this.currentSkip, this.currentLimit);
		}
	}
};
