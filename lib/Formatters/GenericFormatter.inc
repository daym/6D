void print(FILE* destination, Values::NodeT node) {
	if(symbolP(node)) {
		fprintf(destination, "%s", getSymbol1Name(node));
	} else if(keywordP(node)) {
		fprintf(destination, "%s", getKeyword1Name(node));
	} else if(callP(node)) {
		fprintf(destination, "(");
		print(destination, getCallCallable(node));
		fprintf(destination, " ");
		print(destination, getCallArgument(node));
		fprintf(destination, ")");
	} else if(fnP(node)) {
		fprintf(destination, "(\\");
		print(destination, getFnParameter(node));
		fprintf(destination, " ");
		print(destination, getFnBody(node));
		fprintf(destination, ")");
	} else if(consP(node)) {
		fprintf(destination, "(");
		print(destination, getConsHead(node));
		fprintf(destination, " ");
		Values::NodeT tail = getConsTail(node);
		if(consP(tail)) {
			print(destination, getConsHead(tail));
			fprintf(destination, " ");
			print(destination, getConsTail(tail));
		} else
			print(destination, getConsTail(node));
		fprintf(destination, ")");
	} else {
		Values::str(node, destination);
	}
}
