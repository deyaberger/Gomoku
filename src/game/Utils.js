export function from_2d_to_nb(i, j)
{
	let nb = (Number(i) * 19) + Number(j);
	return nb;
}

export function from_reverse_nb_to_2d(reverse_nb)
{
	let i = Math.floor(Number(reverse_nb) / 19);
	let j = Number(reverse_nb) % 19;
	i = Math.abs(18 - i);
	j = Math.abs(18 - j);
	return {i, j};
}

export function from_nb_to_2d(nb)
{
	let i = Math.floor(Number(nb) / 19);
	let j = Number(nb) % 19;
	return {i, j};
}