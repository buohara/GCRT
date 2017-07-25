class PointLight
{
	var pos;
	var color;

	constructor(posIn, colorIn)
	{
		pos = posIn;
		color = colorIn;
	}

	get pos()
	{
		return pos;
	}

	get color()
	{
		return color;
	}
}