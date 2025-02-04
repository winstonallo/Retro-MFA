#include "retro.h"

void	cleanup(t_surf *surf, t_imgdata *data)
{
	free_images(surf, surf->image_count);
	t_img_list* current_node = surf->image_list;
	while (current_node != NULL)
	{
		t_img_list* next_node = current_node->next;
		mlx_destroy_image(surf->mlx, current_node->img.img);
		free(current_node);
		current_node = next_node;
	}
	if (data->img)
	{
		mlx_destroy_image(surf->mlx, data->img);
	}
	if (surf->win)
	{
		mlx_destroy_window(surf->mlx, surf->win);
	}
	if (surf->mlx)
	{
		mlx_destroy_display(surf->mlx);
		free(surf->mlx);
	}
}

int	destroygame(void *ptr)
{
	t_surf	*surf;

	surf = (t_surf *)ptr;
	mlx_loop_end(surf->mlx);
	return (0);
}

int	game_loop(t_surf *surf)
{
	if (generate_images(surf) == EXIT_FAILURE)
	{
		cleanup(surf, surf->img);
		exit(EXIT_FAILURE);
	}
	display_image(surf, surf->current_image);
	return (1);
}

static void	hooks_setup(t_surf *surf)
{
	mlx_hook(surf->win, 2, 1L << 0, key_press_handler, surf);
	mlx_hook(surf->win, 17, 1L, destroygame, surf);
	mlx_loop_hook(surf->mlx, game_loop, surf);

}

int	main(int argc, char **argv)
{
	t_imgdata	data;
	t_surf		surf;

	memset(&surf, 0, sizeof(t_surf));
	if (check_arguments(argc, argv))
		return (1);
	if (is_file_invalid(argv[1]))
		return (1);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	memset(&surf, 0, sizeof(t_surf));
	surf.mlx = mlx_init();
	if (!surf.mlx)
		return (1);
	if (init_window(&surf, &data))
	{
		cleanup(&surf, &data);
		return (1);
	}
	hooks_setup(&surf);
	unsigned char pat1[] = {0x6, 0x10, 0x0, 0x0};
	unsigned char pat2[] = {0x7, 0x10, 0x0, 0x0};
	unsigned char pat3[] = {0x4, 0x10, 0x0, 0x0};
	unsigned char *patterns[] = {pat1, pat2, pat3};
	unsigned char **pat = patterns;
	int image_nums = parse_images(	&surf, 
									argv[1], 
									pat, 
									sizeof(patterns)/sizeof(patterns[0]), 
									sizeof(pat1)/sizeof(pat1[0])); 
	if (image_nums < 0)
	{
		cleanup(&surf, &data);
		exit(1);		
	}
	if (image_nums == 0)
	{
		fprintf(stderr, "No recognisable images found.\n");
		cleanup(&surf, &data);
		exit(1);		
	}
	surf.image_count = image_nums;
	mlx_loop(surf.mlx);
	cleanup(&surf, &data);
	return (0);
}
